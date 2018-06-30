#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MAX_COUNTER 100
#define TOKEN_MAX_LEN 100

typedef enum FileExtension 
{
    INVALID, 
    HEADER,
    SOURCE
} FileExtension;



typedef struct CounterNames
{
    int size;
    char names[MAX_COUNTER][TOKEN_MAX_LEN];
    
} CounterNames;

CounterNames counters;

char token[TOKEN_MAX_LEN];
int tIndex = 0;

int depth = 0;


// 0 -> no letters found, if find 'B' goto 1
// 1 -> B found if find 'E' goto 2
// 2 -> E found if find 'G' goto 3
// 3 -> G found if find 'I' goto 4
// 4 -> I found if find 'N' goto 5
// 5 -> N found if find '(' goto 6
// 6 -> here while until we get ) then goto 7
// 7 -> find ; then goto 0

int ParseBegin(int state, char ch)
{
    switch (state)
    {
        case 0:
        {
            if (ch == 'B') 
            {
                return 1; 
            }
        }
        break;
        case 1:
        {
            if (ch == 'E') 
            {
                return 2; 
            }
        }
        break;
        case 2:
        {
            if (ch == 'G') 
            {
                return 3; 
            }
        }
        break;
        case 3:
        {
            if (ch == 'I') 
            {
                return 4; 
            }
        }
        break;
        case 4:
        {
            if (ch == 'N') 
            {
                return 5; 
            }
        }
        break;
        case 5:
        {
            if (ch == '(') 
            {
                return 6; 
            }
        }
        break;
        case 6:
        {
            if (ch == ')') 
            {
                token[tIndex] = 0;
                
                return 7; 
            }
            else 
            {
                token[tIndex] = ch;
                tIndex++;
                return 6;
            }
        }
        break;
        case 7:
        {
            if (ch == ';') 
            {
                tIndex = 0;
                return 0; 
            }
        }
        break;
    }
    
    return 0;
    
}

int ParseEnd(int state, char ch)
{
    switch (state)
    {
        case 0:
        {
            if (ch == 'E') 
            {
                return 1; 
            }
        }
        break;
        case 1:
        {
            if (ch == 'N') 
            {
                return 2; 
            }
        }
        break;
        case 2:
        {
            if (ch == 'D') 
            {
                return 3; 
            }
        }
        break;
        case 3:
        {
            if (ch == '(') 
            {
                return 4; 
            }
        }
        break;
        case 4:
        {
            if (ch == ')') 
            {
                return 5; 
            }
            else 
            {
                return 4;
            }
        }
        break;
        case 5:
        {
            if (ch == ';') 
            {
                return 0; 
            }
        }
    }
    
    return 0;
    
}

void PushToken()
{
    strcpy(counters.names[counters.size], token);
    counters.size++;
}


char* Concat(const char* first, const char* second)
{
    size_t len1 = strlen(first);
    size_t len2 = strlen(second);
    
    
    char* newStr = (char*) malloc(sizeof(char) * (len1 + len2 + 1));
    
    size_t len = len1 + len2;
    
    newStr[len] = '\0';
    
    int index = 0;
    
    while (*first)
    {
        newStr[index] = *first;
        index++;
        first++;
    }
    
    while (*second)
    {
        newStr[index] = *second;
        index++;
        second++;
    }
    
    
    return newStr;
}


FileExtension ValidFile(const char* fileName)
{
    int lastOccurenceOfDot = 0;
    
    int i = 0;
    
    while (fileName[i])
    {
        if (fileName[i] == '.') lastOccurenceOfDot = i;
        i++;
    }
    
    const char* extension = &fileName[lastOccurenceOfDot + 1];
    
    
    if (strcmp(extension, "c") == 0)
    {
        return SOURCE;
    }
    if (strcmp(extension, "h") == 0)
    {
        return HEADER;
    }
    if (strcmp(extension, "cpp") == 0)
    {
        return SOURCE;
    }
    
    
    return INVALID;
    
}


void ProcessFile(const char* source, const char* dest, const char* fileName)
{
    const char* sourceFilePath = Concat(source, fileName);
    const char* destFilePath = Concat(dest, fileName);
    
    FILE* sourceFile = fopen(sourceFilePath, "rb");
    FILE* parsedFile = fopen(destFilePath, "wb");
    
    fseek(sourceFile, 0L, SEEK_END);
    int size = ftell(sourceFile);
    fseek(sourceFile, 0L, SEEK_SET);
    
    
    char* inBuffer = malloc(sizeof(char)* (size + 1));
    
    fread(inBuffer, size, 1, sourceFile);
    inBuffer[size] = 0;
    
    char* outBuffer = malloc(sizeof(char)* (size + 1000));
    
    int index = 0;
    
    char* inBuff = inBuffer;
    
    while (*inBuffer != '\n')
    {
        outBuffer[index] = *inBuffer;
        index++;
        inBuffer++;
        
    }
    
    outBuffer[index] = '\n';
    index++;
    
    const char* includeStmnt = "#include \"PerfHeader.h\"\n\n";
    
    
    while (*includeStmnt)
    {
        outBuffer[index] = *includeStmnt;
        index++;
        includeStmnt++;
    }
    
    int beginState = 0;
    int endState = 0;
    
    while (*inBuffer != '\0')
    {
        
        beginState = ParseBegin(beginState, *inBuffer);
        endState = ParseEnd(endState, *inBuffer);
        
        if (beginState == 7)
        {
            PushToken();
            depth++;
        }
        
        if (endState == 5)
        {
            depth--;
        }
        
        outBuffer[index] = *inBuffer;
        inBuffer++;
        index++;
    }
    
    outBuffer[index] = '\0';
    
    fwrite(outBuffer, index, 1, parsedFile);
    
    free(inBuff);
    
    
    
    
    fclose(sourceFile);
    fclose(parsedFile);
    
    //printf("Source : %s\nDestination : %s\n\n", sourceFilePath, destFilePath);
}

long long ClockFrequency()
{
    LARGE_INTEGER freq;
    
    QueryPerformanceFrequency(&freq);
    
    return freq.QuadPart;
}



void CreatePerfFile(const char* destPath)
{
    const char* destFilePath = Concat(destPath, "PerfHeader.h");
    FILE* file = fopen(destFilePath, "w");
    
    fprintf(file, "#ifndef PERF_H\n#define PERF_H\n");
    
    fprintf(file, "#include <windows.h>\n");
    
    fprintf(file, "#define BEGIN(name) Push(&perfStack, ClockCount())\n\n");
    fprintf(file, "#define END(name) perfData[name].clockCount += (ClockCount() - Pop(&perfStack)), perfData[name].callCount++\n\n");
    
    fprintf(file, "typedef struct PerfStack\n{\nint size;\nlong long clocks[100];\n} PerfStack;\n");
    
    fprintf(file, "PerfStack perfStack;\n");
    
    fprintf(file, "void Push(PerfStack* stack, long long clock)\n{\nstack->clocks[stack->size] = clock;\nstack->size++;\n}\n");
    
    fprintf(file, "long long Pop(PerfStack* stack)\n{\nstack->size--;\nreturn stack->clocks[stack->size];\n}\n");
    
    /*Printing the enums*/
    
    fprintf(file, "long long ClockCount()\n{\nLARGE_INTEGER clock;\nQueryPerformanceCounter(&clock);\nreturn clock.QuadPart;\n}\n");
    
    fprintf(file, "typedef enum PerfCountName\n{\n");
    
    for (int i = 0; i < counters.size; i++)
    {
        fprintf(file, "\t%s,\n", counters.names[i]);
    }
    
    fprintf(file, "} PerfCountName;\n\n");
    
    /*Printing the name*/
    
    fprintf(file, "char* perfNames[] = {\n");
    
    for (int i = 0; i < counters.size; i++)
    {
        fprintf(file, "\t\"%s\",\n", counters.names[i]);
    }
    
    fprintf(file, "};\n\n");
    
    fprintf(file, "long long clockFrequency = %lld;\n\n", ClockFrequency());
    
    fprintf(file, "typedef struct PerfData\n{\nlong long clockCount;\nint callCount;\n} PerfData;\n");
    
    fprintf(file, "PerfData perfData[%d];\n", counters.size);
    
    fprintf(file, "typedef struct PerformanceMetric\n{\nint size;\nchar** names;\nPerfData* data;\n} PerformanceMetric;\n");
    
    fprintf(file, "void FlushPerfMetric()\n{\n");
    
    fprintf(file, "for (int i = 0; i < %d; i++)\n", counters.size);
    fprintf(file, "{\nperfData[i].clockCount = 0;\nperfData[i].callCount = 0;\n}\n}");
    
    fprintf(file, "PerformanceMetric GetPerfMetric()\n{\n");
    
    fprintf(file, "PerformanceMetric metric = {%d, perfNames, perfData};\n", counters.size);
    fprintf(file, "return metric;\n}\n");
    
    
    
    
    
    fprintf(file, "#endif\n");
    
    fclose(file);
}


int main(int argc, char** argv)
{
    
    counters.size = 0;
    
    if (argc < 3)
    {
        printf("Usage: PerformanceParser [Source File Location] [Destination File Location]");
        
        return -1;
    }
    else
    {
        
        
        WIN32_FIND_DATA fileData;
        
        
        const char* filePath = Concat(argv[1], "*.*");
        
        
        const char* sourceFileDir = argv[1];
        const char* destFileDir = argv[2];
        
        
        HANDLE fileHandle = FindFirstFile(filePath, &fileData);
        
        if (fileHandle == INVALID_HANDLE_VALUE)
        {
            printf("File Not Found\n");
            return - 1;
        }
        else
        {
            FileExtension extension;
            if ((extension = ValidFile(fileData.cFileName)) != INVALID)
            {
                printf("Processing %s...\n", fileData.cFileName);
                ProcessFile(sourceFileDir, destFileDir, fileData.cFileName);
            }
            
            while (FindNextFile(fileHandle, &fileData) != 0)
            {
                if ((extension = ValidFile(fileData.cFileName)) != INVALID)
                {
                    //process it
                    printf("Processing %s...\n", fileData.cFileName);
                    ProcessFile(sourceFileDir, destFileDir, fileData.cFileName);
                }
            }
            
        }
        
    }
    
    CreatePerfFile(argv[2]);
    
    return 0;
}

