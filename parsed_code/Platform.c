#include <windows.h>
#include "PerfHeader.h"


#include <windowsx.h>
#include <stdio.h>
#include <xinput.h>
#include <xaudio2.h>
#include "Platform.h"
#include "StandardDefinitions.h"

#define __SerenityLog(message, type, useErrorCode, errorCode) __SerenityLogF(message, type, __LINE__, __FILE__, useErrorCode, errorCode)

/*Make shift hash map*/


typedef struct SerenityWindowMap
{
    int32 size;
    int32* userIndex;
    int32* windowIndex;
} SerenityWindowMap;

void Add(SerenityWindowMap* map, int32 userIndex, int32 windowIndex)
{
    map->size++;
    int32* newUserIndex = malloc(sizeof(int32) * map->size);
    int32* newWindowIndex = malloc(sizeof(int32) * map->size);
    
    for (int i = 0; i < map->size - 1; i++)
    {
        newUserIndex[i] = map->userIndex[i];
        newWindowIndex[i] = map->windowIndex[i];
    }
    
    newUserIndex[map->size - 1] = userIndex;
    newWindowIndex[map->size - 1] = windowIndex;
    
    free(map->userIndex);
    free(map->windowIndex);
    
    map->userIndex = newUserIndex;
    map->windowIndex = newWindowIndex;
}

int32 GetUserIndex(SerenityWindowMap* map, int32 windowIndex)
{
    for (int i = 0; i < map->size; i++)
    {
        if (map->windowIndex[i] == windowIndex)
        {
            return map->userIndex[i];
        }
    }
    
    return -1;
}


int32 GetWindowIndex(SerenityWindowMap* map, int32 userIndex)
{
    for (int i = 0; i < map->size; i++)
    {
        if (map->userIndex[i] == userIndex)
        {
            return map->windowIndex[i];
        }
    }
    
    return -1;
}

void Remove(SerenityWindowMap* map, int32 windowIndex)
{
    map->size--;
    int32* newUserIndex = malloc(sizeof(int32) * map->size);
    int32* newWindowIndex = malloc(sizeof(int32) * map->size);
    
    int32 index = 0;
    
    for (int i = 0; i < map->size + 1; i++)
    {
        if (map->windowIndex[i] != windowIndex)
        {
            newUserIndex[index] = map->userIndex[i];
            newWindowIndex[index] = map->windowIndex[i];
            index++;
        }
    }
    
    free(map->userIndex);
    free(map->windowIndex);
    
    map->userIndex = newUserIndex;
    map->windowIndex = newWindowIndex;
}

/*
void Display(SerenityWindowMap* map)
{
    for (int i = 0; i < map->size; i++)
    {
        printf("%d -> %d\n", map->windowIndex[i], map->userIndex[i]);
    }
    printf("*****************\n");
}
*/
void ChangeWindowIndex(SerenityWindowMap* map, int32 oWindowIndex, int32 nWindowIndex)
{
    for (int32 i = 0; i < map->size; i++)
    {
        if (map->windowIndex[i] == oWindowIndex)
        {
            map->windowIndex[i] = nWindowIndex;
        }
    }
}


//*******************//

typedef enum SerenityMessageType
{
    SR_LOG,
    SR_WARNING,
    SR_ERROR,
    SR_FERROR
    
} SerenityMessageType;

typedef struct SerenityLog
{
    char filePath[250];
    char buffer[4096];
    int size;
    int capacity;
    
} SerenityLog;

typedef struct SerenityWindow    // window specific window struct
{
    /* Handle */
    HWND windowHandle;
    
    /* Window & Sound */
    BITMAPINFO bitMapInfo;
    WAVEFORMATEX waveInfo;
    SerenityGraphicBuffer screenBuffer;
    SerenitySoundBuffer soundBuffer;
    RenderCallbackFunction* RenderCallback;
    
    /* State Info */
    boolean running;
    int32 prevX, prevY;
    int32 prevWidth, prevHeight;
    
} SerenityWindow;


typedef struct SerenityController
{
    boolean currUp, currDown, currLeft, currRight;
    boolean prevUp, prevDown, prevLeft, prevRight;
    boolean currStart, currSelect;
    boolean prevStart, prevSelect;
    boolean currLThumb, currRThumb;
    boolean prevLThumb, prevRThumb;
    boolean currLeftShoulder, currRightShoulder;
    boolean prevLeftShoulder, prevRightShoulder;
    boolean currA, currB, currX, currY;
    boolean prevA, prevB, prevX, prevY;
    uint8 leftTrigger, rightTrigger;
    int16 lAnalogX, lAnalogY;
    int16 rAnalogX, rAnalogY;
    
} SerenityController;

typedef SerenityWindow* SerenityWindowPointer;


typedef struct SerenityEnvironment // TODO(Suhaib): Thread safety please and check for dirty cache problems
{
    int32 numberOfWindows; 
    int32 currentUserIndex;
    SerenityWindowMap map;
    SerenityWindowPointer *windows;
    int32 activeWindowIndex;
    int32 mouseHoverWindowIndex;
    float64 queryPerfFrequency; 
    SerenityLog log;
    
    /*Keyboard*/
    boolean keyPressed[256]; // TODO(Suhaib): Possibly query the system for these lengths
    boolean keyTyped[256];
    boolean keyReleased[256];
    /*Mouse*/
    int32 xPos, yPos;
    int32 mouseWheelRotationDelta;
    int8 buttonPressed[8];
    int8 buttonClicked[8];
    int8 buttonReleased[8];
    
    /*Controller*/
    SerenityController controllers[XUSER_MAX_COUNT];
    
} SerenityEnvironment;

global SerenityEnvironment environment;

int32 AddWindow() //returns window index
{
    if (environment.windows == NULL)
    {
        environment.windows = malloc(sizeof(SerenityWindowPointer));
        int32 windowIndex = environment.numberOfWindows++;
        int32 userIndex = environment.currentUserIndex++;
        Add(&environment.map, userIndex, windowIndex);
        
        environment.windows[0] = malloc(sizeof(SerenityWindow));
        
        return 0;
    }
    else
    {
        SerenityWindowPointer* newWindows = malloc(sizeof(SerenityWindowPointer) * (environment.numberOfWindows + 1));
        int32 windowIndex = environment.numberOfWindows++;
        int32 userIndex = environment.currentUserIndex++;
        Add(&environment.map, userIndex, windowIndex);
        
        int i;
        
        for (i = 0; i < environment.numberOfWindows - 1; i++)
        {
            newWindows[i] = environment.windows[i];
        }
        
        newWindows[i] = malloc(sizeof(SerenityWindow));
        
        free(environment.windows);
        
        environment.windows = newWindows;
        
        return i;
        
    }
}

void RemoveWindow(int windowIndex)
{
    SerenityWindowPointer* newWindows = (environment.numberOfWindows == 1 ? NULL : malloc(sizeof(SerenityWindowPointer) * (environment.numberOfWindows - 1)));
    
    int newIndex = 0;
    
    Remove(&environment.map, windowIndex);
    
    for (int i = 0; i < environment.numberOfWindows; i++)
    {
        if (i != windowIndex)
        {
            ChangeWindowIndex(&environment.map, i, newIndex);
            newWindows[newIndex] = environment.windows[i]; 
            newIndex++;
        }
    }
    
    
    
    free(environment.windows);
    
    environment.numberOfWindows--;
    
    environment.windows = newWindows;
    
    //Display(&environment.map);
}


typedef DWORD __SerenityGetControllerStateSignature(DWORD , XINPUT_STATE* ); // typedef to a function signature
typedef DWORD __SerenitySetControllerStateSignature(DWORD , XINPUT_VIBRATION* );


global __SerenityGetControllerStateSignature* __SerenityGetControllerState; // function pointer to XInputGetState
global __SerenitySetControllerStateSignature* __SerenitySetControllerState; // function pointer to XInputSetState


/*----------------------------Private Functions Declarations-------------------------------*/

internal int32 __SerenityGetWindowIndexFromHandle(HWND windowHandle);
internal LRESULT CALLBACK __SerenityWindowCallback(HWND   windowHandle, UINT   message, WPARAM wParam, LPARAM lParam);
internal void __SerenityStubRenderFunction(SerenityGraphicBuffer gBuffer);
internal void __SerenityDeallocateWindowResources(int32 windowIndex);
internal DWORD __SerenityGetControllerStateStub(DWORD index, XINPUT_STATE* state);
internal DWORD __SerenitySetControllerStateStub(DWORD index, XINPUT_VIBRATION* vibration);
internal void __SerenityLogF(char* message, SerenityMessageType type, int line, const char* file, boolean useErrorCode, uint32 errorCode);
internal void __SerenityDumpLogToFile();

/*----------------------------Private Functions-------------------------------*/

// getting the slot index of a window with the given window handle
internal int32 __SerenityGetWindowIndexFromHandle(HWND windowHandle)
{
    for (int32 i = 0; i < environment.numberOfWindows; i++)
    {
        if (windowHandle == environment.windows[i]->windowHandle) 
        {
            return i;
        }
    }
    
    return -1;
}


// windows callback function
internal LRESULT CALLBACK __SerenityWindowCallback(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    
    int32 windowIndex = __SerenityGetWindowIndexFromHandle(windowHandle); 
    
    if (windowIndex != -1) // if no matches let DefWindowProc handle everything
    {
        
        switch(message)
        {
            case WM_CLOSE: 
            {
                DestroyWindow(windowHandle); // sends a destroy message
            }
            break;
            case WM_QUIT:
            {
                DestroyWindow(windowHandle); // sends a destroy message
            }
            break;
            case WM_DESTROY:
            {
                __SerenityDeallocateWindowResources(windowIndex); // get the destroy message sent by the previous 2 and actual destroys the window
            }
            break;
            case WM_ACTIVATE: 
            {
                if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE) 
                {
                    environment.activeWindowIndex = windowIndex;
                }
                else if (wParam == WA_INACTIVE)
                {
                    if (windowIndex == environment.activeWindowIndex) 
                    {
                        environment.activeWindowIndex = -1; 
                    }
                }
            }
            break;
            case WM_SIZE:
            {
                RECT clientRectangle = (RECT) {0};
                GetClientRect(windowHandle, &clientRectangle);
                
                int32 width = (int32 ) (clientRectangle.right - clientRectangle.left);
                int32 height = (int32 ) (clientRectangle.bottom - clientRectangle.top);
                
                SerenityWindow* window = environment.windows[windowIndex];
                
                window->bitMapInfo.bmiHeader.biSize = sizeof(window->bitMapInfo.bmiHeader);
                window->bitMapInfo.bmiHeader.biWidth = width;
                window->bitMapInfo.bmiHeader.biHeight = height;
                window->bitMapInfo.bmiHeader.biPlanes = 1;
                window->bitMapInfo.bmiHeader.biBitCount = 32;
                window->bitMapInfo.bmiHeader.biCompression = BI_RGB;
                window->bitMapInfo.bmiHeader.biSizeImage = 0;
                window->bitMapInfo.bmiHeader.biXPelsPerMeter = 0;
                window->bitMapInfo.bmiHeader.biYPelsPerMeter = 0;
                window->bitMapInfo.bmiHeader.biClrUsed = 0;
                window->bitMapInfo.bmiHeader.biClrImportant = 0;
                
            }
            break;
            default:
            {
                result = DefWindowProc(windowHandle, message, wParam, lParam);
            }
        }
    }
    else 
    {
        result = DefWindowProc(windowHandle, message, wParam, lParam);
    }
    return result;
}


// a default render function that is assigned as a callback is the user doesn't provide its own
// renders a black screen
internal void __SerenityStubRenderFunction(SerenityGraphicBuffer gBuffer)
{
    memset(gBuffer.buffer, 0xee,  gBuffer.width * gBuffer.height * gBuffer.bitsPerPixel / 8);
}


internal void __SerenityDeallocateWindowResources(int32 windowIndex) 
{
    environment.windows[windowIndex]->running = 0;
    
    
    
    // TODO(Suhaib): Check if memory free was succesfull else I guess just crash
    free(environment.windows[windowIndex]->screenBuffer.buffer);
    free(environment.windows[windowIndex]->soundBuffer.buffer);
    
    RemoveWindow(windowIndex);
}

internal DWORD __SerenityGetControllerStateStub(DWORD index, XINPUT_STATE* state) // stub function is xinput not there
{
    return ERROR_DEVICE_NOT_CONNECTED; // basically if no xinput then give failure state
}

internal DWORD __SerenitySetControllerStateStub(DWORD index, XINPUT_VIBRATION* vibration) // stub function is xinput not there
{
    return ERROR_DEVICE_NOT_CONNECTED;
}



internal void __SerenityLogF(char* message, SerenityMessageType type, int line, const char* file, boolean useErrorCode, uint32 errorCode)
{
    
    char logMsg[1024];
    
    int len = 0;
    
    switch (type)
    {
        case SR_LOG:
        {
            if (useErrorCode)
                len = sprintf(logMsg, "[LOG]          : %s with Error Code : %u at Line %d in File %s\n", message, errorCode, line, file);
            else 
                len = sprintf(logMsg, "[LOG]          : %s at Line %d in File %s\n", message, line, file);
        }
        break;
        case SR_WARNING:
        {
            if (useErrorCode)
                len = sprintf(logMsg, "[WARNING]      : %s with MSDN Error Code : %u at Line %d in File %s\n", message, errorCode, line, file);
            else 
                len = sprintf(logMsg, "[WARNING]      : %s at Line %d in File %s\n", message, line, file);
        }
        break;
        case SR_ERROR:
        {
            if (useErrorCode)
                len = sprintf(logMsg, "[ERROR]        : %s with MSDN Error Code : %u at Line %d in File %s\n", message, errorCode, line, file);
            else 
                len = sprintf(logMsg, "[ERROR]        : %s at Line %d in File %s\n", message, line, file);
        }
        break;
        case SR_FERROR:
        {
            if (useErrorCode)
                len = sprintf(logMsg, "[FATAL ERROR]  : %s with MSDN Error Code : %u  at Line %d in File %s\n", message, errorCode, line, file);
            else 
                len = sprintf(logMsg, "[FATAL ERROR]  : %s at Line %d in File %s\n", message, line, file);
        }
        break;
    }
    
    int size = environment.log.size;
    int capacity = environment.log.capacity;
    
    
    if (size + len  >= capacity)
    {
        __SerenityDumpLogToFile();
    }
    
    for (int i = 0; i < len; i++)
    {
        environment.log.buffer[size] = logMsg[i];
        size++;
    }
    
    environment.log.size = size;
    
#ifdef SERENITY_EXITONWARNING
    if (type == SR_WARNING)
    {
        SerenityClose(-1);
    }
#endif
#ifdef SERENITY_EXITONERROR
    if (type == SR_ERROR)
    {
        SerenityClose(-1);
    }
#endif
    
    if (type == SR_FERROR)
    {
        SerenityClose(-1);
    }
}

internal void __SerenityDumpLogToFile()
{
    FILE* file = fopen(environment.log.filePath, "w");
    
    environment.log.buffer[environment.log.size] = '\0';
    
    fputs(environment.log.buffer, file);
    
}

/*------------------------Public Functions--------------------------------*/

/*---------------------Window Creation Functions------------------------*/

void SerenityInitialize()
{
#ifdef SERENITY_NOCONSOLE
    FreeConsole();
#endif
    
    environment = (SerenityEnvironment) {0};
    
    /******Initializing the path of log file*******/
    
    char str[250];
    
    GetModuleFileName(NULL, str, 250); // get the file path of the executable
    
    char* ch = &str[0];
    
    int lastIndexOfSlash = -1;
    int index = 0;
    
    while (*ch) // find out the last index of \ example S:\build\Main.exe we are only concerened with S:\build\ 
    {
        if (*ch == '\\')
        {
            lastIndexOfSlash = index;
        }
        index++;
        ch++;
        
    }
    
    int currIndex = 0;
    
    for (currIndex = 0; currIndex <= lastIndexOfSlash; currIndex++) // copy till the last index of \ to the log file path
    {
        environment.log.filePath[currIndex] = str[currIndex];
    }
    
    char logFileName[] = "SerenityLog.txt";
    
    ch = &logFileName[0];
    
    while (*ch) // appending SerenityLog.txt to the log file path making S:\build\SerenityLog.txt
    {
        environment.log.filePath[currIndex] = *ch;
        currIndex++;
        ch++;
    }
    
    environment.log.filePath[currIndex] = '\0';
    
    /**********************************************/
    
    
    environment.activeWindowIndex = -1;
    environment.mouseHoverWindowIndex = -1;
    
    environment.log.size = 0;
    environment.log.capacity = 4096;
    
    LARGE_INTEGER freq;
    
    QueryPerformanceFrequency(&freq);
    
    environment.queryPerfFrequency = (float64) freq.QuadPart;
    
    //load xinput
    HMODULE xInputDLL = LoadLibrary("xinput1_4.dll"); // try loading the latest version
    
    if (xInputDLL == NULL) // not found
    {
        xInputDLL = LoadLibrary("xinput1_3.dll"); // recursively try to load earlier versions
        
        if (xInputDLL == NULL)
        {
            xInputDLL = LoadLibrary("xinput9_1_0.dll");
        }
    }
    
    if (xInputDLL != NULL)
    {
        __SerenityGetControllerState = (__SerenityGetControllerStateSignature*) GetProcAddress(xInputDLL, "XInputGetState");
        __SerenitySetControllerState = (__SerenitySetControllerStateSignature*) GetProcAddress(xInputDLL, "XInputSetState");
    }
    else
    {
        __SerenityLog("XInput Not Available can't use controllers", SR_WARNING, 0, 0);
        __SerenityGetControllerState =  __SerenityGetControllerStateStub;
        __SerenitySetControllerState =  __SerenitySetControllerStateStub;
    }
    
    
}

void SerenityClose(int exitCode)
{
    __SerenityDumpLogToFile();
    if (exitCode != 0) exit(exitCode);
}

// creating a new window in one of the empty slots, if not slots available contiue the program and log the message
SerenityError SerenityCreateWindow(int32* userIndex, 
                                   int32 width, int32 height, 
                                   const char* windowName, 
                                   RenderCallbackFunction* renderCallback)
{
    
    HINSTANCE instance = GetModuleHandle(NULL);
    
    WNDCLASSEX windowClass = (WNDCLASSEX) {0}; // Creating a window class
    
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
    windowClass.lpfnWndProc = __SerenityWindowCallback;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = instance;
    windowClass.hIcon = NULL; // TODO(Suhaib): Add an icon
    windowClass.hCursor = NULL; // TODO(Suhaib): Add a cursor
    windowClass.hbrBackground = NULL; // We will paint our backgrounds our selves
    windowClass.lpszMenuName = NULL; // No default menu
    windowClass.lpszClassName = windowName;
    windowClass.hIconSm = NULL;
    
    if (RegisterClassEx(&windowClass) != 0) // TODO(Suhaib): Maybe check unicode vs ASCII
    {
        HWND windowHandle = CreateWindowEx(
            0,
            windowName,
            windowName,
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            width, height, 
            NULL,
            NULL,
            instance,
            NULL
            );
        
        if (windowHandle != NULL)
        {
            *userIndex = environment.currentUserIndex;
            
            int windowSlot = AddWindow();
            
            SerenityWindow* window = environment.windows[windowSlot];
            
            
            window->bitMapInfo.bmiHeader.biSize = sizeof(window->bitMapInfo.bmiHeader);
            window->bitMapInfo.bmiHeader.biWidth = width;
            window->bitMapInfo.bmiHeader.biHeight = height;
            window->bitMapInfo.bmiHeader.biPlanes = 1;
            window->bitMapInfo.bmiHeader.biBitCount = 32;
            window->bitMapInfo.bmiHeader.biCompression = BI_RGB;
            window->bitMapInfo.bmiHeader.biSizeImage = 0;
            window->bitMapInfo.bmiHeader.biXPelsPerMeter = 0;
            window->bitMapInfo.bmiHeader.biYPelsPerMeter = 0;
            window->bitMapInfo.bmiHeader.biClrUsed = 0;
            window->bitMapInfo.bmiHeader.biClrImportant = 0;
            
            
            window->screenBuffer.width = width;
            window->screenBuffer.height = height;
            window->screenBuffer.bitsPerPixel = 32;
            
            window->screenBuffer.buffer = malloc(width * height * 32 / 8);
            
            
            
            window->waveInfo.wFormatTag = WAVE_FORMAT_PCM;
            window->waveInfo.nChannels = 2;
            window->waveInfo.nSamplesPerSec = 48000;
            window->window.nAvgBytesPerSec = 48000 * 2;
            window->waveInfo.nBlockAlign = 2 * 16 / 2;
            window->waveInfo.wBitsPerSample = 16;
            window->waveInfo.cbSize = 0;
            
            window->screenBuffer.width = width;
            window->screenBuffer.height = height;
            
            if (window->screenBuffer.buffer != NULL)
            {
                free(window->screenBuffer.buffer);
            }
            
            window->screenBuffer.buffer = malloc(width * height * 32 / 8);
            
            
            /*Audio Stuff to change*/
            window->soundBuffer.buffer = malloc(48000 * 2 * 2);
            
            uint16* soundPlayer = (uint16*) window->soundBuffer.buffer;
            
            uint16 value = 0;
            
            for (int i = 0; i < 48000; i++)
            {
                if (i % 48 == 0)
                {
                    value = (value == 0 ? 10000 : 0);
                }
                soundPlayer[2 * i] = value;
                soundPlayer[2 * i + 1] = value;
            }
            
            
            XAUDIO2_BUFFER xAudioBuffer = (XAUDIO2_BUFFER) {0};
            xAudioBuffer.Flags = 0;
            xAudioBuffer.AudioBytes = 48000 * 2 * 2;
            xAudioBuffer.pAudioData = (BYTE*) window->soundBuffer.buffer;
            xAudioBuffer.PlayBegin = 0;
            xAudioBuffer.PlayLength = 0;
            xAudioBuffer.LoopBegin = 0;
            xAudioBuffer.LoopLength = 48000;
            xAudioBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
            xAudioBuffer.pContext = 0;
            
            IXAudio2* pXAudio2 = NULL;
            HRESULT hr;
            if ( FAILED(hr = XAudio2Create( &pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) )
                return SERENITY_WINDOWCREATIONFAILED;
            
            IXAudio2MasteringVoice* pMasterVoice = NULL;
            if ( FAILED(hr = pXAudio2->CreateMasteringVoice( &pMasterVoice ) ) )
                return SERENITY_WINDOWCREATIONFAILED;
            
            IXAudio2SourceVoice* pSourceVoice;
            if( FAILED(hr = pXAudio2->CreateSourceVoice( &pSourceVoice, (WAVEFORMATEX*)&wfx ) ) ) 
                return SERENITY_WINDOWCREATIONFAILED;
            
            if( FAILED(hr = pSourceVoice->SubmitSourceBuffer( &buffer ) ) )
                return SERENITY_WINDOWCREATIONFAILED;
            
            if ( FAILED(hr = pSourceVoice->Start( 0 ) ) )
                return SERENITY_WINDOWCREATIONFAILED;
            
            /*Audio Stuff to change*/
            
            
            window->running = 1;
            window->windowHandle = windowHandle;
            
            environment.activeWindowIndex = windowSlot;
            
            window->RenderCallback = renderCallback;
            
            return SERENITY_SUCCESS;
            
        }
        else
        {
            DWORD ec = GetLastError();
            __SerenityLog("Window Creation Failed", SR_ERROR, 1, ec);
            return SERENITY_WINDOWCREATIONFAILED;
        }
        
    }
    else 
    {
        DWORD ec = GetLastError();
        __SerenityLog("Class Registration Failed", SR_ERROR, 1, ec);
        return SERENITY_CLASSREGISTERATIONFAILED;
    }
    
    
    
}

// returns true is no windows currently active (ie running) in the application
boolean SerenityApplicationShouldClose()
{
    return environment.numberOfWindows == 0;
}


void __SerenityToggleFullscreen(SerenityWindow* window)
{
    DWORD style = GetWindowLong(window->windowHandle, GWL_STYLE);
    
    if (style & WS_OVERLAPPEDWINDOW) // not already full screen
    {
        
        RECT windowRect;
        
        GetClientRect(window->windowHandle, &windowRect);
        
        // TODO(Suhaib): This is not correct cause left and top are always 0
        window->prevX = windowRect.left;
        window->prevY = windowRect.top;
        
        
        window->prevWidth = windowRect.right - windowRect.left;
        window->prevHeight = windowRect.bottom - windowRect.top;
        
        // TODO(Suhaib): Multi Monitor support
        int width = GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);
        
        SetWindowLong(window->windowHandle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(window->windowHandle, HWND_TOP, 0, 0, width, height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
    else
    {
        SetWindowLong(window->windowHandle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        
        
        SetWindowPos(window->windowHandle, HWND_TOP, window->prevX, window->prevY, window->prevWidth, window->prevHeight, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
    
}


void SerenityToggleFullscreen(int32 userIndex)
{
    int windowIndex = GetWindowIndex(&environment.map, userIndex);
    __SerenityToggleFullscreen(environment.windows[windowIndex]);
}


// updates every event. MessageQueue, Input, etc.
void SerenityUpdate()
{
    MSG message;
    
    
    BEGIN(InputResetPref);
    
    environment.mouseWheelRotationDelta = 0;
    
    
    for (int i = 0; i < 256; i++)
    {
        environment.keyReleased[i] = 0;
        environment.keyTyped[i] = 0;
    }
    
    for (int i = 0; i < 8; i++)
    {
        environment.buttonReleased[i] = 0;
        environment.buttonClicked[i] = 0;
    }
    
    END(InputResetPref);
    
    BEGIN(XInputPerf);
    
    //Controller (XInput) update
    XINPUT_STATE xiState;
    XINPUT_GAMEPAD xiGamepad;
    DWORD xiResult;
    
    for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
    {
        xiState = (XINPUT_STATE) {0};
        
        xiResult = __SerenityGetControllerState(i, &xiState);
        
        xiGamepad = xiState.Gamepad;
        
        if (xiResult == ERROR_SUCCESS)
        {
            environment.controllers[i].prevUp = environment.controllers[i].currUp;
            environment.controllers[i].currUp = (xiGamepad.wButtons & 0x0001) != 0;
            
            environment.controllers[i].prevDown = environment.controllers[i].currDown;
            environment.controllers[i].currDown = (xiGamepad.wButtons & 0x0002) != 0;
            
            environment.controllers[i].prevLeft = environment.controllers[i].currLeft;
            environment.controllers[i].currLeft = (xiGamepad.wButtons & 0x0004) != 0;
            
            environment.controllers[i].prevRight = environment.controllers[i].currRight;
            environment.controllers[i].currRight = (xiGamepad.wButtons & 0x0008) != 0;
            
            environment.controllers[i].prevStart = environment.controllers[i].currStart;
            environment.controllers[i].currStart = (xiGamepad.wButtons & 0x0010) != 0;
            
            environment.controllers[i].prevSelect = environment.controllers[i].currSelect;
            environment.controllers[i].currSelect = (xiGamepad.wButtons & 0x0020) != 0;
            
            environment.controllers[i].prevLThumb = environment.controllers[i].currLThumb;
            environment.controllers[i].currLThumb = (xiGamepad.wButtons & 0x0040) != 0;
            
            environment.controllers[i].prevRThumb = environment.controllers[i].currRThumb;
            environment.controllers[i].currRThumb = (xiGamepad.wButtons & 0x0080) != 0;
            
            environment.controllers[i].prevLeftShoulder = environment.controllers[i].currLeftShoulder;
            environment.controllers[i].currLeftShoulder = (xiGamepad.wButtons & 0x0100) != 0;
            
            environment.controllers[i].prevRightShoulder = environment.controllers[i].currRightShoulder;
            environment.controllers[i].currRightShoulder = (xiGamepad.wButtons & 0x0200) != 0;
            
            environment.controllers[i].prevA = environment.controllers[i].currA;
            environment.controllers[i].currA = (xiGamepad.wButtons & 0x1000) != 0;
            
            environment.controllers[i].prevB = environment.controllers[i].currB;
            environment.controllers[i].currB = (xiGamepad.wButtons & 0x2000) != 0;
            
            environment.controllers[i].prevX = environment.controllers[i].currX;
            environment.controllers[i].currX = (xiGamepad.wButtons & 0x4000) != 0;
            
            environment.controllers[i].prevY = environment.controllers[i].currY;
            environment.controllers[i].currY = (xiGamepad.wButtons & 0x8000) != 0;
            
            environment.controllers[i].leftTrigger = xiGamepad.bLeftTrigger;
            environment.controllers[i].rightTrigger = xiGamepad.bRightTrigger; 
            
            environment.controllers[i].lAnalogX = xiGamepad.sThumbLX;
            environment.controllers[i].lAnalogY = xiGamepad.sThumbLY;
            
            environment.controllers[i].rAnalogX = xiGamepad.sThumbRX;
            environment.controllers[i].rAnalogY = xiGamepad.sThumbRY;
        }
    }
    
    END(XInputPerf);
    
    
    for (int32 i = 0; i < environment.numberOfWindows; i++)
    {
        BEGIN(WindowUpdatePerf);
        SerenityWindow* window = environment.windows[i];
        
        if (window->running)
        {
            while (PeekMessage(&message, window->windowHandle, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&message);
                
                switch (message.message)
                {
                    case WM_KEYDOWN: // a key is pressed
                    {
                        if (environment.keyPressed[message.wParam] != 1)
                        {
                            environment.keyTyped[message.wParam] = 1;
                        }
                        
                        environment.keyPressed[message.wParam] = 1;
                    }
                    break;
                    case WM_KEYUP:
                    {
                        environment.keyPressed[message.wParam] = 0;
                        environment.keyTyped[message.wParam] = 0;
                        environment.keyReleased[message.wParam] = 1;
                    }
                    break;
                    case WM_SYSKEYDOWN: // NOTE(Suhaib): Note SYS KEY UP and DOWN are basically the same as normal ones except we need to handle the same of ALT
                    {
                        
#ifdef SERENITY_ALTF4CLOSE
                        if (message.wParam == SR_F4)
                        {
                            DestroyWindow(window->windowHandle);
                        }
#endif
                        
#ifdef SERENITY_ALTF5FULLSCREEN
                        if (message.wParam == SR_F5)
                        {
                            __SerenityToggleFullscreen(window);
                        }
#endif
                        if (environment.keyPressed[message.wParam] != 1)
                        {
                            environment.keyTyped[message.wParam] = 1;
                        }
                        
                        environment.keyPressed[message.wParam] = 1;
                        
                        if (environment.keyPressed[SR_ALT] != 1)
                        {
                            environment.keyTyped[SR_ALT] = 1;
                        }
                        
                        environment.keyPressed[SR_ALT] = 1;
                        
                    }
                    break;
                    case WM_SYSKEYUP:
                    {
                        environment.keyPressed[message.wParam] = 0;
                        environment.keyTyped[message.wParam] = 0;
                        environment.keyReleased[message.wParam] = 1;
                        
                        
                        
                        environment.keyPressed[SR_ALT] = 0;
                        environment.keyTyped[SR_ALT] = 0;
                        environment.keyReleased[SR_ALT] = 1;
                    }
                    break;
                    
                    case WM_MOUSEMOVE:
                    {
                        environment.xPos = GET_X_LPARAM(message.lParam); 
                        environment.yPos = GET_Y_LPARAM(message.lParam); 
                        environment.mouseHoverWindowIndex = GetUserIndex(&environment.map, i);
                    }
                    break;
                    
                    case WM_MOUSEWHEEL:
                    {
                        environment.mouseWheelRotationDelta = GET_WHEEL_DELTA_WPARAM(message.wParam) / 120;
                        
                    }
                    break;
                    
                    case WM_LBUTTONDOWN:
                    {
                        
                        if (environment.buttonPressed[SR_LBUTTON] != 1)
                        {
                            environment.buttonClicked[SR_LBUTTON] = 1;
                        }
                        
                        environment.buttonPressed[SR_LBUTTON] = 1;
                    }
                    break;
                    case WM_LBUTTONUP:
                    {
                        environment.buttonPressed[SR_LBUTTON] = 0;
                        environment.buttonClicked[SR_LBUTTON] = 0;
                        environment.buttonReleased[SR_LBUTTON] = 1;
                    }
                    break;
                    case WM_RBUTTONDOWN:
                    {
                        if (environment.buttonPressed[SR_RBUTTON] != 1)
                        {
                            environment.buttonClicked[SR_RBUTTON] = 1;
                        }
                        
                        environment.buttonPressed[SR_RBUTTON] = 1;
                    }
                    break;
                    case WM_RBUTTONUP:
                    {
                        environment.buttonPressed[SR_RBUTTON] = 0;
                        environment.buttonClicked[SR_RBUTTON] = 0;
                        environment.buttonReleased[SR_RBUTTON] = 1;
                    }
                    break;
                    case WM_MBUTTONDOWN:
                    {
                        if (environment.buttonPressed[SR_MBUTTON] != 1)
                        {
                            environment.buttonClicked[SR_MBUTTON] = 1;
                        }
                        
                        environment.buttonPressed[SR_MBUTTON] = 1;
                    }
                    break;
                    case WM_MBUTTONUP:
                    {
                        environment.buttonPressed[SR_MBUTTON] = 0;
                        environment.buttonClicked[SR_MBUTTON] = 0;
                        environment.buttonReleased[SR_MBUTTON] = 1;
                    }
                    break;
                    
                    default:
                    {
                        DispatchMessage(&message);
                    }
                }
                
            }
        }
        END(WindowUpdatePerf);
    }
    
}

void SerenityUpdateInput()
{
    
    environment.mouseWheelRotationDelta = 0;
    
    for (int i = 0; i < 256; i++)
    {
        environment.keyReleased[i] = 0;
        environment.keyTyped[i] = 0;
    }
    
    for (int i = 0; i < 8; i++)
    {
        environment.buttonReleased[i] = 0;
        environment.buttonClicked[i] = 0;
    }
    
    
    //Controller (XInput) update
    XINPUT_STATE xiState;
    XINPUT_GAMEPAD xiGamepad;
    DWORD xiResult;
    
    for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
    {
        xiState = (XINPUT_STATE) {0};
        
        xiResult = __SerenityGetControllerState(i, &xiState);
        
        xiGamepad = xiState.Gamepad;
        
        if (xiResult == ERROR_SUCCESS)
        {
            environment.controllers[i].prevUp = environment.controllers[i].currUp;
            environment.controllers[i].currUp = (xiGamepad.wButtons & 0x0001) != 0;
            
            environment.controllers[i].prevDown = environment.controllers[i].currDown;
            environment.controllers[i].currDown = (xiGamepad.wButtons & 0x0002) != 0;
            
            environment.controllers[i].prevLeft = environment.controllers[i].currLeft;
            environment.controllers[i].currLeft = (xiGamepad.wButtons & 0x0004) != 0;
            
            environment.controllers[i].prevRight = environment.controllers[i].currRight;
            environment.controllers[i].currRight = (xiGamepad.wButtons & 0x0008) != 0;
            
            environment.controllers[i].prevStart = environment.controllers[i].currStart;
            environment.controllers[i].currStart = (xiGamepad.wButtons & 0x0010) != 0;
            
            environment.controllers[i].prevSelect = environment.controllers[i].currSelect;
            environment.controllers[i].currSelect = (xiGamepad.wButtons & 0x0020) != 0;
            
            environment.controllers[i].prevLThumb = environment.controllers[i].currLThumb;
            environment.controllers[i].currLThumb = (xiGamepad.wButtons & 0x0040) != 0;
            
            environment.controllers[i].prevRThumb = environment.controllers[i].currRThumb;
            environment.controllers[i].currRThumb = (xiGamepad.wButtons & 0x0080) != 0;
            
            environment.controllers[i].prevLeftShoulder = environment.controllers[i].currLeftShoulder;
            environment.controllers[i].currLeftShoulder = (xiGamepad.wButtons & 0x0100) != 0;
            
            environment.controllers[i].prevRightShoulder = environment.controllers[i].currRightShoulder;
            environment.controllers[i].currRightShoulder = (xiGamepad.wButtons & 0x0200) != 0;
            
            environment.controllers[i].prevA = environment.controllers[i].currA;
            environment.controllers[i].currA = (xiGamepad.wButtons & 0x1000) != 0;
            
            environment.controllers[i].prevB = environment.controllers[i].currB;
            environment.controllers[i].currB = (xiGamepad.wButtons & 0x2000) != 0;
            
            environment.controllers[i].prevX = environment.controllers[i].currX;
            environment.controllers[i].currX = (xiGamepad.wButtons & 0x4000) != 0;
            
            environment.controllers[i].prevY = environment.controllers[i].currY;
            environment.controllers[i].currY = (xiGamepad.wButtons & 0x8000) != 0;
            
            environment.controllers[i].leftTrigger = xiGamepad.bLeftTrigger;
            environment.controllers[i].rightTrigger = xiGamepad.bRightTrigger; 
            
            environment.controllers[i].lAnalogX = xiGamepad.sThumbLX;
            environment.controllers[i].lAnalogY = xiGamepad.sThumbLY;
            
            environment.controllers[i].rAnalogX = xiGamepad.sThumbRX;
            environment.controllers[i].rAnalogY = xiGamepad.sThumbRY;
        }
    }
    
    
    
    
}


void SerenityUpdateWindow(int32 userIndex)
{
    
    MSG message;
    
    int windowIndex = GetWindowIndex(&environment.map, userIndex);
    
    SerenityWindow* window = environment.windows[windowIndex];
    
    
    if (window->running)
    {
        while (PeekMessage(&message, window->windowHandle, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            
            switch (message.message) 
            {
                case WM_KEYDOWN: // a key is pressed
                {
                    if (environment.keyPressed[message.wParam] != 1)
                    {
                        environment.keyTyped[message.wParam] = 1;
                    }
                    
                    environment.keyPressed[message.wParam] = 1;
                }
                break;
                case WM_KEYUP:
                {
                    environment.keyPressed[message.wParam] = 0;
                    environment.keyTyped[message.wParam] = 0;
                    environment.keyReleased[message.wParam] = 1;
                }
                break;
                case WM_SYSKEYDOWN: // NOTE(Suhaib): Note SYS KEY UP and DOWN are basically the same as normal ones except we need to handle the same of ALT
                {
                    
#ifdef SERENITY_ALTF4CLOSE
                    if (message.wParam == SR_F4)
                    {
                        DestroyWindow(window->windowHandle);
                    }
#endif
                    
#ifdef SERENITY_ALTF5FULLSCREEN
                    if (message.wParam == SR_F5)
                    {
                        __SerenityToggleFullscreen(window);
                    }
#endif
                    if (environment.keyPressed[message.wParam] != 1)
                    {
                        environment.keyTyped[message.wParam] = 1;
                    }
                    
                    environment.keyPressed[message.wParam] = 1;
                    
                    if (environment.keyPressed[SR_ALT] != 1)
                    {
                        environment.keyTyped[SR_ALT] = 1;
                    }
                    
                    environment.keyPressed[SR_ALT] = 1;
                    
                }
                break;
                case WM_SYSKEYUP:
                {
                    environment.keyPressed[message.wParam] = 0;
                    environment.keyTyped[message.wParam] = 0;
                    environment.keyReleased[message.wParam] = 1;
                    
                    environment.keyPressed[SR_ALT] = 0;
                    environment.keyTyped[SR_ALT] = 0;
                    environment.keyReleased[SR_ALT] = 1;
                }
                break;
                
                case WM_MOUSEMOVE:
                {
                    environment.xPos = GET_X_LPARAM(message.lParam); 
                    environment.yPos = GET_Y_LPARAM(message.lParam); 
                    environment.mouseHoverWindowIndex = GetUserIndex(&environment.map, userIndex);
                }
                break;
                
                case WM_MOUSEWHEEL:
                {
                    environment.mouseWheelRotationDelta = GET_WHEEL_DELTA_WPARAM(message.wParam) / 120;
                    
                }
                break;
                case WM_LBUTTONDOWN:
                {
                    
                    if (environment.buttonPressed[SR_LBUTTON] != 1)
                    {
                        environment.buttonClicked[SR_LBUTTON] = 1;
                    }
                    
                    environment.buttonPressed[SR_LBUTTON] = 1;
                }
                break;
                case WM_LBUTTONUP:
                {
                    environment.buttonPressed[SR_LBUTTON] = 0;
                    environment.buttonClicked[SR_LBUTTON] = 0;
                    environment.buttonReleased[SR_LBUTTON] = 1;
                }
                break;
                case WM_RBUTTONDOWN:
                {
                    if (environment.buttonPressed[SR_RBUTTON] != 1)
                    {
                        environment.buttonClicked[SR_RBUTTON] = 1;
                    }
                    
                    environment.buttonPressed[SR_RBUTTON] = 1;
                }
                break;
                case WM_RBUTTONUP:
                {
                    environment.buttonPressed[SR_RBUTTON] = 0;
                    environment.buttonClicked[SR_RBUTTON] = 0;
                    environment.buttonReleased[SR_RBUTTON] = 1;
                }
                break;
                case WM_MBUTTONDOWN:
                {
                    if (environment.buttonPressed[SR_MBUTTON] != 1)
                    {
                        environment.buttonClicked[SR_MBUTTON] = 1;
                    }
                    
                    environment.buttonPressed[SR_MBUTTON] = 1;
                }
                break;
                case WM_MBUTTONUP:
                {
                    environment.buttonPressed[SR_MBUTTON] = 0;
                    environment.buttonClicked[SR_MBUTTON] = 0;
                    environment.buttonReleased[SR_MBUTTON] = 1;
                }
                break;
                
                
                
                default:
                {
                    DispatchMessage(&message);
                }
            }
            
        }
    }
}

void SerenityRender()
{
    for (int i = 0; i < environment.numberOfWindows; i++)
    {
        
        SerenityWindow* window = environment.windows[i];
        
        if (window->running)
        {
            BEGIN(BufferFillPerf);
            window->RenderCallback(window->screenBuffer);
            END(BufferFillPerf);
            BEGIN(DisplayPerf);
            StretchDIBits(
                GetDC(window->windowHandle),
                0, 0,
                window->screenBuffer.width, window->screenBuffer.height,
                0, 0,
                window->screenBuffer.width, window->screenBuffer.height,
                window->screenBuffer.buffer,
                &window->bitMapInfo,
                DIB_RGB_COLORS, SRCCOPY
                );
            END(DisplayPerf);
        }
    }
}

void SerenityRenderWindow(int userIndex)
{
    int windowIndex = GetWindowIndex(&environment.map, userIndex);
    
    SerenityWindow* window = environment.windows[windowIndex];
    
    if (window->running)
    {
        
        window->RenderCallback(window->screenBuffer);
        
        StretchDIBits(
            GetDC(window->windowHandle),
            0, 0,
            window->screenBuffer.width, window->screenBuffer.height,
            0, 0,
            window->screenBuffer.width, window->screenBuffer.height,
            window->screenBuffer.buffer,
            &window->bitMapInfo,
            DIB_RGB_COLORS, SRCCOPY
            );
        
    }
}

void SerenityCloseWindow(int32 userIndex)
{
    int windowIndex = GetWindowIndex(&environment.map, userIndex);
    
    PostMessage(environment.windows[windowIndex]->windowHandle, WM_QUIT, 0, 0);
}

inline SerenityController SerenityGetControllerState(int controllerIndex)
{
    return environment.controllers[controllerIndex];
}

inline boolean SerenityKeyPressed(SerenityVirtualKey virtualKeyCode)
{
    return environment.keyPressed[virtualKeyCode];
}

inline boolean SerenityKeyTyped(SerenityVirtualKey virtualKeyCode)
{
    return environment.keyTyped[virtualKeyCode];
}

inline boolean SerenityKeyReleased(SerenityVirtualKey virtualKeyCode)
{
    return environment.keyReleased[virtualKeyCode];
}

inline SerenityMousePosition SerenityGetMousePosition()
{
    return (SerenityMousePosition) {environment.mouseHoverWindowIndex, environment.xPos, environment.yPos};
}

inline boolean SerenityButtonClicked(SerenityVirtualMouseButton virtualButtonCode)
{
    return environment.buttonClicked[virtualButtonCode];
}

inline boolean SerenityButtonReleased(SerenityVirtualMouseButton virtualButtonCode)
{
    return environment.buttonReleased[virtualButtonCode];
}

inline boolean SerenityButtonPressed(SerenityVirtualMouseButton virtualButtonCode)
{
    return environment.buttonPressed[virtualButtonCode];
}

inline int32 SerenityGetMouseWheelDelta()
{
    return environment.mouseWheelRotationDelta;
}

inline SerenityClock SerenityStartTimer()
{
    LARGE_INTEGER time;
    
    QueryPerformanceCounter(&time);
    
    return (SerenityClock) {(float64) time.QuadPart};
}

inline float64 SerenityElapsedTime(SerenityClock clock)
{
    LARGE_INTEGER time;
    
    QueryPerformanceCounter(&time);
    
    return ((float64) time.QuadPart  - clock.startQueryPerfCount) / environment.queryPerfFrequency;
}

inline boolean SerenityControllerButtonPressed(int index, SerenityControllerButton buttonCode)
{
    switch (buttonCode)
    {
        case SR_DUP:            return environment.controllers[index].currUp;   
        case SR_DDOWN:          return environment.controllers[index].currDown;   
        case SR_DLEFT:          return environment.controllers[index].currLeft;   
        case SR_DRIGHT:         return environment.controllers[index].currRight;   
        case SR_CSTART:         return environment.controllers[index].currStart;   
        case SR_CSELECT:        return environment.controllers[index].currSelect;   
        case SR_LTHUMB:        return environment.controllers[index].currLThumb;   
        case SR_RTHUMB:        return environment.controllers[index].currRThumb;   
        case SR_LSHOULDER:     return environment.controllers[index].currLeftShoulder;   
        case SR_RSHOULDER:     return environment.controllers[index].currRightShoulder;   
        case SR_A:             return environment.controllers[index].currA;   
        case SR_B:             return environment.controllers[index].currB;   
        case SR_X:             return environment.controllers[index].currX;   
        case SR_Y:             return environment.controllers[index].currY;   
        default:               return 0;
    }
    
}

inline boolean SerenityControllerButtonClicked(int index, SerenityControllerButton buttonCode)
{
    switch (buttonCode)
    {
        case SR_DUP:            return environment.controllers[index].currUp == 1 && environment.controllers[index].prevUp == 0;   
        case SR_DDOWN:          return environment.controllers[index].currDown == 1 && environment.controllers[index].prevDown == 0;   
        case SR_DLEFT:          return environment.controllers[index].currLeft == 1 && environment.controllers[index].prevLeft == 0;   
        case SR_DRIGHT:         return environment.controllers[index].currRight == 1 && environment.controllers[index].prevRight == 0;   
        case SR_CSTART:         return environment.controllers[index].currStart == 1 && environment.controllers[index].prevStart == 0;   
        case SR_CSELECT:        return environment.controllers[index].currSelect == 1 && environment.controllers[index].prevSelect == 0;   
        case SR_LTHUMB:        return environment.controllers[index].currLThumb == 1 && environment.controllers[index].prevLThumb == 0;
        case SR_RTHUMB:        return environment.controllers[index].currRThumb == 1 && environment.controllers[index].prevRThumb == 0;   
        case SR_LSHOULDER:     return environment.controllers[index].currLeftShoulder == 1 && environment.controllers[index].prevLeftShoulder == 0;   
        case SR_RSHOULDER:     return environment.controllers[index].currRightShoulder == 1 && environment.controllers[index].prevRightShoulder == 0;   
        case SR_A:             return environment.controllers[index].currA == 1 && environment.controllers[index].prevA == 0;   
        case SR_B:             return environment.controllers[index].currB == 1 && environment.controllers[index].prevB == 0;
        case SR_X:             return environment.controllers[index].currX == 1 && environment.controllers[index].prevX == 0;   
        case SR_Y:             return environment.controllers[index].currY == 1 && environment.controllers[index].prevY == 0;   
        default:               return 0;
    }
    
}


inline boolean SerenityControllerButtonReleased(int index, SerenityControllerButton buttonCode)
{
    switch (buttonCode)
    {
        case SR_DUP:            return environment.controllers[index].currUp == 0 && environment.controllers[index].prevUp == 1;   
        case SR_DDOWN:          return environment.controllers[index].currDown == 0 && environment.controllers[index].prevDown == 1;   
        case SR_DLEFT:          return environment.controllers[index].currLeft == 0 && environment.controllers[index].prevLeft == 1;   
        case SR_DRIGHT:         return environment.controllers[index].currRight == 0 && environment.controllers[index].prevRight == 1;   
        case SR_CSTART:         return environment.controllers[index].currStart == 0 && environment.controllers[index].prevStart == 1;   
        case SR_CSELECT:        return environment.controllers[index].currSelect == 0 && environment.controllers[index].prevSelect == 1;   
        case SR_LTHUMB:        return environment.controllers[index].currLThumb == 0 && environment.controllers[index].prevLThumb == 1;
        case SR_RTHUMB:        return environment.controllers[index].currRThumb == 0 && environment.controllers[index].prevRThumb == 1;   
        case SR_LSHOULDER:     return environment.controllers[index].currLeftShoulder == 0 && environment.controllers[index].prevLeftShoulder == 1;   
        case SR_RSHOULDER:     return environment.controllers[index].currRightShoulder == 0 && environment.controllers[index].prevRightShoulder == 1;   
        case SR_A:             return environment.controllers[index].currA == 0 && environment.controllers[index].prevA == 1;   
        case SR_B:             return environment.controllers[index].currB == 0 && environment.controllers[index].prevB == 1;
        case SR_X:             return environment.controllers[index].currX == 0 && environment.controllers[index].prevX == 1;   
        case SR_Y:             return environment.controllers[index].currY == 0 && environment.controllers[index].prevY == 1;   
        default:               return 0;
    }
    
}

inline uint8 SerenityControllerLeftTrigger(int index)
{
    return environment.controllers[index].leftTrigger;
}

inline uint8 SerenityControllerRightTrigger(int index)
{
    return environment.controllers[index].rightTrigger;
}

inline SerenityCoordinate SerenityControllerLeftAnalog(int index)
{
    return (SerenityCoordinate) { environment.controllers[index].lAnalogX, environment.controllers[index].lAnalogY };
}

inline SerenityCoordinate SerenityControllerRightAnalog(int index)
{
    return (SerenityCoordinate) { environment.controllers[index].rAnalogX, environment.controllers[index].rAnalogY };
}

inline void SerenityControllerVibrate(uint32 index, uint16 leftMotor, uint16 rightMotor)
{
    XINPUT_VIBRATION vibration = (XINPUT_VIBRATION) {leftMotor, rightMotor};
    __SerenitySetControllerState(index, &vibration);
}