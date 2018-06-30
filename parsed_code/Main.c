int frameNumber = 0;
#include "PerfHeader.h"


#define SERENITY_ALTF5FULLSCREEN 
#define SERENITY_EXITONERROR
#define SERENITY_EXITONWARNING
#include "Platform.c"

#include <stdio.h>
#include "StandardDefinitions.h"
#include <malloc.h>

global uint32 color = 0xff00ff00;

void RenderingFunction0(SerenityGraphicBuffer gBuffer)
{
    int32* buff = gBuffer.buffer;
    
    for (int i = 0; i < gBuffer.width * gBuffer.height * gBuffer.bitsPerPixel / 32; i++)
    {
        *buff = 0xffffffff;
        buff++;
    }
    
}

void RenderingFunction1(SerenityGraphicBuffer gBuffer)
{
    int32* buff = gBuffer.buffer;
    
    for (int i = 0; i < gBuffer.width * gBuffer.height * gBuffer.bitsPerPixel / 32; i++)
    {
        *buff = (color + 0xff);
        buff++;
    }
    
}

void RenderingFunction2(SerenityGraphicBuffer gBuffer)
{
    int32* buff = gBuffer.buffer;
    
    for (int i = 0; i < gBuffer.width * gBuffer.height * gBuffer.bitsPerPixel / 32; i++)
    {
        *buff = (color + 0xff00);
        buff++;
    }
    
}

void RenderingFunction3(SerenityGraphicBuffer gBuffer)
{
    int32* buff = gBuffer.buffer;
    
    for (int i = 0; i < gBuffer.width * gBuffer.height * gBuffer.bitsPerPixel / 32; i++)
    {
        *buff = (color + 0xffff);
        buff++;
    }
    
}

void RenderingFunction4(SerenityGraphicBuffer gBuffer)
{
    int32* buff = gBuffer.buffer;
    
    for (int i = 0; i < gBuffer.width * gBuffer.height * gBuffer.bitsPerPixel / 32; i++)
    {
        *buff = (color + 0xff0000);
        buff++;
    }
    
}

void RenderingFunction5(SerenityGraphicBuffer gBuffer)
{
    int32* buff = gBuffer.buffer;
    
    for (int i = 0; i < gBuffer.width * gBuffer.height * gBuffer.bitsPerPixel / 32; i++)
    {
        *buff = (color + 0xff00ff);
        buff++;
    }
    
}

void PrintPerformance()
{
    
    PerformanceMetric metric = GetPerfMetric();
    
    for (int i = 0; i < metric.size; i++)
    {
        
        printf("Name : %s\nTotal Cycle Count : %lld\nTotal Call Count : %d\n\n", metric.names[i], metric.data[i].clockCount, metric.data[i].callCount);
        
    }
    
    FlushPerfMetric();
    
}

int main(int32 argc, char** argv)
{
    
    
    SerenityInitialize();
    
    int32 win1;
    
    SerenityError code;
    
    code = SerenityCreateWindow(&win1, 1280, 720, "SerenityWindow1", RenderingFunction1);
    
#if 0
    int win2, win3, win4, win5;
    code = SerenityCreateWindow(&win2, 760, 500, "SerenityWindow2", RenderingFunction2);
    code = SerenityCreateWindow(&win3, 1280, 720, "SerenityWindow3", RenderingFunction3);
    code = SerenityCreateWindow(&win4, 300, 200, "SerenityWindow4", RenderingFunction4);
    code = SerenityCreateWindow(&win5, 400, 800, "SerenityWindow5", RenderingFunction5);
#endif
    
    
    char str[] = "Hello1";
    
    while (!SerenityApplicationShouldClose())
    {
        BEGIN(MainPerf);
        BEGIN(UpdatePerf);
        SerenityUpdate();
        END(UpdatePerf);
        BEGIN(RenderPerf);
        SerenityRender();
        END(RenderPerf);
        
        if (SerenityKeyTyped(SR_F4) && SerenityKeyPressed(SR_ALT))
        {
            break;
        }
        
        //printf("A : %d\n", environment.keyPressCount[PK_KEY_A]);
        
        if (SerenityControllerButtonReleased(0, SR_DUP))
        {
            
            SerenityControllerVibrate(0, 10000, 10000);
        }
        
        if (SerenityControllerButtonReleased(0, SR_DDOWN))
        {
            SerenityControllerVibrate(0, 0, 0);
        }
        
        
        if (SerenityKeyTyped(SR_KEY_A))
        {
            int win;
            code = SerenityCreateWindow(&win, 1280, 720, str, RenderingFunction0);
            str[5]++;
        }
        
        color += (uint32) (10 * SerenityGetMouseWheelDelta());
        
#if 0
        printf("Up              %d\n", controller.up);
        printf("Down            %d\n", controller.down);
        printf("Left            %d\n", controller.left);
        printf("Right           %d\n", controller.right);
        printf("Start           %d\n", controller.start);
        printf("Select          %d\n", controller.select);
        printf("Right Thumb     %d\n", controller.rThumb);
        printf("Left Thumb      %d\n", controller.lThumb);
        printf("Left Shoulder   %d\n", controller.leftShoulder);
        printf("Left Shoulder   %d\n", controller.rightShoulder);
        printf("A               %d\n", controller.a);
        printf("B               %d\n", controller.b);
        printf("X               %d\n", controller.x);
        printf("Y               %d\n", controller.y);
        printf("Left Trigger    %d\n", controller.leftTrigger);
        printf("Right Trigger   %d\n", controller.rightTrigger);
        printf("Left Analog X   %d\n", controller.lAnalogX);
        printf("Left Analog Y   %d\n", controller.lAnalogY);
        printf("Right Analog X  %d\n", controller.rAnalogX);
        printf("Right Analog Y  %d\n", controller.rAnalogY);
        
        system("cls");
#endif
        
        END(MainPerf);
        
        PrintPerformance();
        printf("***************************\n");
    }
    
    SerenityClose(0);
    
    
    
    
    return 0;
}