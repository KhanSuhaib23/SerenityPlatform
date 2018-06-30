#ifndef SERENITY_PLATFORM
#define SERENITY_PLATFORM

#include "StandardDefinitions.h"

/*Virtual Key Codes*/
typedef enum SerenityVirtualKey
{
    SR_NULL                       = 0x00,  //null
    SR_CANCEL	                 = 0x03,  //Break
    SR_BACK	                   = 0x08,  //Backspace
    SR_TAB	                    = 0x09,  //Tab
    SR_CLEAR	                  = 0x0C,  //Clear
    SR_RETURN	                 = 0x0D,  //Enter
    SR_SHIFT                      = 0x10,  //any shift
    SR_CONTROL                    = 0x11,  //any control
    SR_ALT                        = 0x12,  //any alt
    SR_PAUSE                      = 0x13,  //Pause
    SR_CAPITAL                    = 0x14,  //Caps Lock
    SR_KANA                       = 0x15,  //Kana
    SR_JUNJA                      = 0x17,  //Junja
    SR_FINAL                      = 0x18,  //Final
    SR_KANJI                      = 0x19,  //Kanji
    SR_ESCAPE	                 = 0x1B,  //Esc
    SR_CONVERT                    = 0x1C,  //Convert
    SR_NONCONVERT                 = 0x1D,  //Non Convert
    SR_ACCEPT                     = 0x1E,  //Accept
    SR_MODECHANGE                 = 0x1F,  //Mode Change
    SR_SPACE	                  = 0x20,  //Space
    SR_PRIOR                      = 0x21,  //Page Up
    SR_NEXT                       = 0x22,  //Page Down
    SR_END                        = 0x23,  //End
    SR_HOME                       = 0x24,  //Home
    SR_LEFT                       = 0x25,  //Arrow Left
    SR_UP                         = 0x26,  //Arrow Up
    SR_RIGHT                      = 0x27,  //Arrow Right
    SR_DOWN                       = 0x28,  //Arrow Down
    SR_SELECT	                 = 0x29,  //Select
    SR_PRINT                      = 0x2A,  //Print
    SR_EXECUTE	                = 0x2B,  //Execute
    SR_SNAPSHOT                   = 0x2C,  //Print Screen
    SR_INSERT                     = 0x2D,  //Insert
    SR_DELETE                     = 0x2E,  //Delete
    SR_HELP                       = 0x2F,  //Help
    SR_KEY_0	                  = 0x30,  //('0')	0
    SR_KEY_1	                  = 0x31,  //('1')	1
    SR_KEY_2	                  = 0x32,  //('2')	2
    SR_KEY_3	                  = 0x33,  //('3')	3
    SR_KEY_4	                  = 0x34,  //('4')	4
    SR_KEY_5	                  = 0x35,  //('5')	5
    SR_KEY_6	                  = 0x36,  //('6')	6
    SR_KEY_7	                  = 0x37,  //('7')	7
    SR_KEY_8	                  = 0x38,  //('8')	8
    SR_KEY_9	                  = 0x39,  //('9')	9
    SR_KEY_A	                  = 0x41,  //('A')	A
    SR_KEY_B	                  = 0x42,  //('B')	B
    SR_KEY_C	                  = 0x43,  //('C')	C
    SR_KEY_D	                  = 0x44,  //('D')	D
    SR_KEY_E	                  = 0x45,  //('E')	E
    SR_KEY_F	                  = 0x46,  //('F')	F
    SR_KEY_G	                  = 0x47,  //('G')	G
    SR_KEY_H	                  = 0x48,  //('H')	H
    SR_KEY_I	                  = 0x49,  //('I')	I
    SR_KEY_J	                  = 0x4A,  //('J')	J
    SR_KEY_K	                  = 0x4B,  //('K')	K
    SR_KEY_L	                  = 0x4C,  //('L')	L
    SR_KEY_M	                  = 0x4D,  //('M')	M
    SR_KEY_N	                  = 0x4E,  //('N')	N
    SR_KEY_O	                  = 0x4F,  //('O')	O
    SR_KEY_P	                  = 0x50,  //('P')	P
    SR_KEY_Q	                  = 0x51,  //('Q')	Q
    SR_KEY_R	                  = 0x52,  //('R')	R
    SR_KEY_S	                  = 0x53,  //('S')	S
    SR_KEY_T	                  = 0x54,  //('T')	T
    SR_KEY_U	                  = 0x55,  //('U')	U
    SR_KEY_V	                  = 0x56,  //('V')	V
    SR_KEY_W	                  = 0x57,  //('W')	W
    SR_KEY_X	                  = 0x58,  //('X')	X
    SR_KEY_Y	                  = 0x59,  //('Y')	Y
    SR_KEY_Z	                  = 0x5A,  //('Z')	Z
    SR_LWIN                       = 0x5B,  //Left Win
    SR_RWIN                       = 0x5C,  //Right Win
    SR_APPS                       = 0x5D,  //Context Menu
    SR_SLEEP                      = 0x5F,  //Sleep
    SR_NUMPAD0	                = 0x60,  //Numpad 0
    SR_NUMPAD1	                = 0x61,  //Numpad 1
    SR_NUMPAD2	                = 0x62,  //Numpad 2
    SR_NUMPAD3	                = 0x63,  //Numpad 3
    SR_NUMPAD4	                = 0x64,  //Numpad 4
    SR_NUMPAD5	                = 0x65,  //Numpad 5
    SR_NUMPAD6	                = 0x66,  //Numpad 6
    SR_NUMPAD7	                = 0x67,  //Numpad 7
    SR_NUMPAD8	                = 0x68,  //Numpad 8
    SR_NUMPAD9	                = 0x69,  //Numpad 9
    SR_MULTIPLY	               = 0x6A,  //Numpad *
    SR_ADD	                    = 0x6B,  //Numpad +
    SR_SEPARATOR	              = 0x6C,  //Separator
    SR_SUBTRACT	               = 0x6D,  //Num -
    SR_DECIMAL	                = 0x6E,  //Numpad .
    SR_DIVIDE	                 = 0x6F,  //Numpad /
    SR_F1                         = 0x70,  //F1
    SR_F2                         = 0x71,  //F2
    SR_F3                         = 0x72,  //F3
    SR_F4                         = 0x73,  //F4
    SR_F5                         = 0x74,  //F5
    SR_F6                         = 0x75,  //F6
    SR_F7                         = 0x76,  //F7
    SR_F8                         = 0x77,  //F8
    SR_F9                         = 0x78,  //F9
    SR_F10                        = 0x79,  //F10
    SR_F11                        = 0x7A,  //F11
    SR_F12                        = 0x7B,  //F12
    SR_F13                        = 0x7C,  //F13
    SR_F14                        = 0x7D,  //F14
    SR_F15                        = 0x7E, //F15 
    SR_F16                        = 0x7F,  //F16
    SR_F17                        = 0x80,  //F17
    SR_F18                        = 0x81,  //F18
    SR_F19                        = 0x82,  //F19
    SR_F20                        = 0x83,  //F20
    SR_F21                        = 0x84,  //F21
    SR_F22                        = 0x85,  //F22
    SR_F23                        = 0x86,  //F23
    SR_F24                        = 0x87,  //F24
    SR_NUMLOCK                    = 0x90,  //Num Lock
    SR_SCROLL                     = 0x91,  //Scrol Lock
    SR_OEM_FJ_JISHO               = 0x92,  //Jisho
    SR_OEM_FJ_MASSHOU	         = 0x93,  //Mashu
    SR_OEM_FJ_TOUROKU	         = 0x94,  //Touroku
    SR_OEM_FJ_LOYA	            = 0x95,  //Loya
    SR_OEM_FJ_ROYA	            = 0x96,  //Roya
    SR_LSHIFT                     = 0xA0,  //Left Shift
    SR_RSHIFT                     = 0xA1,  //Right Shift
    SR_LCONTROL                   = 0xA2,  //Left Ctrl
    SR_RCONTROL                   = 0xA3,  //Right Ctrl
    SR_LALT                       = 0xA4,  //Left Alt
    SR_RALT                       = 0xA5,  //Right Alt
    SR_BROWSER_BACK               = 0xA6,  //Browser Back
    SR_BROWSER_FORWARD            = 0xA7,  //Browser Forward
    SR_BROWSER_REFRESH            = 0xA8,  //Browser Refresh
    SR_BROWSER_STOP               = 0xA9,  //Browser Stop
    SR_BROWSER_SEARCH             = 0xAA,  //Browser Search
    SR_BROWSER_FAVORITES          = 0xAB,  //Browser Favorites
    SR_BROWSER_HOME               = 0xAC,  //Browser Home
    SR_VOLUME_MUTE                = 0xAD,  //Volume Mute
    SR_VOLUME_DOWN                = 0xAE,  //Volume Down
    SR_VOLUME_UP                  = 0xAF,  //Volume Up
    SR_MEDIA_NEXT_TRACK           = 0xB0,  //Next Track
    SR_MEDIA_PREV_TRACK           = 0xB1,  //Previous Track
    SR_MEDIA_STOP                 = 0xB2,  //Stop
    SR_MEDIA_PLAY_PAUSE           = 0xB3,  //Play / Pause
    SR_LAUNCH_MAIL                = 0xB4,  //Mail
    SR_LAUNCH_MEDIA_SELECT        = 0xB5,  //Media
    SR_LAUNCH_APP1                = 0xB6,  //App1
    SR_LAUNCH_APP2                = 0xB7,  //App2
    SR_OEM_1	                  = 0xBA,  //OEM_1 (: ;)
    SR_OEM_PLUS	               = 0xBB,  //OEM_PLUS (+ =)
    SR_OEM_COMMA	              = 0xBC,  //OEM_COMMA (< ,)
    SR_OEM_MINUS	              = 0xBD,  //OEM_MINUS (_ -)
    SR_OEM_PERIOD	             = 0xBE,  //OEM_PERIOD (> .)
    SR_OEM_2	                  = 0xBF,  //OEM_2 (? /)
    SR_OEM_3	                  = 0xC0,  //OEM_3 (~ `)
    SR_ABNT_C1	                = 0xC1,  //Abnt C1
    SR_ABNT_C2	                = 0xC2,  //Abnt C2
    SR_OEM_4	                  = 0xDB,  //OEM_4 ({ [)
    SR_OEM_5	                  = 0xDC,  //OEM_5 (| \)
    SR_OEM_6	                  = 0xDD,  //OEM_6 (} ])
    SR_OEM_7	                  = 0xDE,  //OEM_7 (" ')
    SR_OEM_8	                  = 0xDF,  //OEM_8 (Â§ !)
    SR_OEM_AX	                 = 0xE1,  //Ax
    SR_OEM_102	                = 0xE2,  //OEM_102 (> <)
    SR_ICO_HELP	               = 0xE3, //IcoHlp
    SR_ICO_00                     = 0xE4, //Ico00 *
    SR_PROCESSKEY	             = 0xE5,  //Process
    SR_ICO_CLEAR	              = 0xE6,  //IcoClr
    SR_PACKET	                 = 0xE7,  //Packet
    SR_OEM_RESET	              = 0xE9,  //Reset
    SR_OEM_JUMP 	              = 0xEA,  //Jump
    SR_OEM_PA1	                = 0xEB,  //OemPa1
    SR_OEM_PA2	                = 0xEC,  //OemPa2
    SR_OEM_PA3	                = 0xED,  //OemPa3
    SR_OEM_WSCTRL	             = 0xEE,  //WsCtrl
    SR_OEM_CUSEL	              = 0xEF, //Cu Sel
    SR_OEM_ATTN	               = 0xF0,  //Oem Attn
    SR_OEM_FINISH	             = 0xF1,  //Finish
    SR_OEM_COPY	               = 0xF2,  //Copy
    SR_OEM_AUTO	               = 0xF3,  //Auto
    SR_OEM_ENLW	               = 0xF4,  //Enlw
    SR_OEM_BACKTAB	            = 0xF5,  //Back Tab
    SR_ATTN	                   = 0xF6,  //Attn
    SR_CRSEL	                  = 0xF7,  //Cr Sel
    SR_EXSEL	                  = 0xF8,  //Ex Sel
    SR_EREOF	                  = 0xF9,  //Er Eof
    SR_PLAY	                   = 0xFA,  //Play
    SR_ZOOM	                   = 0xFB,  //Zoom
    SR_NONAME	                 = 0xFC,  //
    SR_PA1	                    = 0xFD,  //Pa1
    SR_OEM_CLEAR	              = 0xFE,  //OemClr
    SR__none_                     = 0xFF  //no VK mapping
    
} SerenityVirtualKey;



// Mouse Virtual Buttons
typedef enum SerenityVirtualMouseButton
{
    SR_LBUTTON  = 0,
    SR_RBUTTON  = 1,
    SR_MBUTTON  = 2
    
} SerenityVirtualMouseButton;

typedef enum SerenityControllerButton
{
    SR_DUP, SR_DDOWN, SR_DLEFT, SR_DRIGHT,
    SR_CSTART, SR_CSELECT,
    SR_LTHUMB, SR_RTHUMB,
    SR_LSHOULDER, SR_RSHOULDER,
    SR_A, SR_B, SR_X, SR_Y
    
} SerenityControllerButton;

typedef struct SerenityMousePosition
{
    int32 windowIndex;
    int32 xPos, yPos;
    
} SerenityMousePosition;

typedef struct SerenityCoordinate
{
    int xPos, yPos;
    
} SerenityCoordinate;

typedef struct SerenityClock
{
    float64 startQueryPerfCount;
} SerenityClock;


typedef struct SerenityGraphicBuffer
{
    int32 width, height;
    int32 bitsPerPixel;
    void* buffer;
    
} SerenityGraphicBuffer;

typedef struct SerenitySoundBuffer
{
    int32 numberOfSamples;
    int32 numberOfChannels;
    int32 bitsPerSample;
    void* buffer;
    
} SerenitySoundBuffer;



typedef enum SerenityError
{
    SERENITY_SUCCESS = 0,
    SERENITY_NOMOREAVAILABLESLOTS = 1,
    SERENITY_WINDOWCREATIONFAILED = 2,
    SERENITY_CLASSREGISTERATIONFAILED = 3,
    SERENITY_MEMORYALLOCFAILED = 4,
    SERENITY_MEMORYFREEFAILED = 5
} SerenityError;

typedef void (RenderCallbackFunction(SerenityGraphicBuffer gBuffer)); //typedef for type of function expected for RenderCallback

void SerenityInitialize();
void SerenityClose(int exitCode);
SerenityError SerenityCreateWindow(int32* userIndex, int32 width, int32 height, const char* windowName, RenderCallbackFunction* renderCallback);
boolean SerenityApplicationShouldClose();
void SerenityUpdate();
void SerenityUpdateInput();
void SerenityUpdateWindow(int32 userIndex);
void SerenityRender();
void SerenityRenderWindow(int userIndex);
void SerenityCloseWindow(int32 userIndex);
inline boolean SerenityIsKeyPressed(int32 virtualKeyCode);
inline boolean SerenityWasKeyTyped(int32 virtualKeyCode);
inline boolean SerenityWasKeyReleased(int32 virtualKeyCode);
inline SerenityMousePosition SerenityGetMousePosition();
inline boolean SerenityWasButtonClicked(int32 virtualButtonCode);
inline boolean SerenityWasButtonReleased(int32 virtualButtonCode);
inline boolean SerenityIsButtonPressed(int32 virtualButtonCode);
inline int32 SerenityGetMouseWheelDelta();
inline SerenityClock SerenityStartTimer();
inline float64 SerenityElapsedTime(SerenityClock clock);
#endif