#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h" //Memory mapping of the used device (maps all the pins ports etc. to specific numbers)
#include "driverlib/fpu.h" //Routines for manipulating the floating-point unit in the Cortex-M processor.
#include "driverlib/sysctl.h" //Driver for the system controller
#include "driverlib/rom.h"		//Macros to facilitate calling functions in the ROM
#include "driverlib/pin_map.h" //Mapping of peripherals to pins for all parts
#include "driverlib/uart.h" 	// Driver for the UART
#include "grlib/grlib.h"	//Prototypes for the low level primitives provided by the graphics library
#include "drivers/ili9341_240x320x262K.h" //Display driver for the MULTI-INNO TECHNOLOGY
										// MI0283QT-9 TFT display with an ILI9341 controller.
#include "utils/uartstdio.h"		//Prototypes for the UART console functions.
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "drivers/touch.h"



#define GPIO_PINS_ALL GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7



//*****************************************************************************
//
// Global system tick counter holds elapsed time since the application started
// expressed in 100ths of a second.
//
//*****************************************************************************
volatile uint32_t g_ui32SysTickCount;

int_fast32_t i32CenterX, i32CenterY;
//int i32CenterX_temp = 100;
//int i32CenterY_temp = 100;

tContext sContext;
tRectangle sRect;

unsigned long color = ClrBlack;
bool type = true;
long size = 5;

//*****************************************************************************
//
// This is the interrupt handler for the SysTick interrupt.  It is used to
// update our local tick count which, in turn, is used to check for transmit
// timeouts.
//
//*****************************************************************************
void SysTickIntHandler(void)
{
    g_ui32SysTickCount++;
}


//*****************************************************************************
//
//! Callback for calibration process
//!
//! \param X - x coordinate of point to check
//! \param Y - y coordinate of point to check
//! \param cX - x coordinate of circle center
//! \param cY - y coordinate of circle center
//! \param radius - circle radius
//!
//! \return boolean true if coordinates are within circle
//
//*****************************************************************************
inline bool IsInCircle(int32_t X,int32_t Y,int32_t cX,int32_t cY,int32_t radius)
{
	return ( (X-cX)*(X-cX) + (Y-cY)*(Y-cY) < (radius*radius) );
}



//*****************************************************************************
//
//! Callback for calibration process
//!
//! \param ulMessage is type of event
//! \param lX is a x location of touch
//! \param lY is a y location of cross center
//!
//! \return None.
//
//*****************************************************************************
void drawMenu() {
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = 320;
    sRect.i16YMax = 40;
    GrContextForegroundSet(&sContext, ClrBlue);
    GrRectFill(&sContext, &sRect);

    sRect.i16XMin = 0;
    sRect.i16YMin = 200;
    sRect.i16XMax = 320;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrBlue);
    GrRectFill(&sContext, &sRect);

    //Kolory
    
    sRect.i16XMin = 10;
    sRect.i16YMin = 10;
    sRect.i16XMax = 30;
    sRect.i16YMax = 30;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);

    drawOutline(10, 30, 10, 30);

    sRect.i16XMin = 50;
    sRect.i16YMin = 10;
    sRect.i16XMax = 70;
    sRect.i16YMax = 30;
    GrContextForegroundSet(&sContext, ClrRed);
    GrRectFill(&sContext, &sRect);

    drawOutline(50, 70, 10, 30);

    sRect.i16XMin = 90;
    sRect.i16YMin = 10;
    sRect.i16XMax = 110;
    sRect.i16YMax = 30;
    GrContextForegroundSet(&sContext, ClrYellow);
    GrRectFill(&sContext, &sRect);

    drawOutline(90, 110, 10, 30);

    sRect.i16XMin = 130;
    sRect.i16YMin = 10;
    sRect.i16XMax = 150;
    sRect.i16YMax = 30;
    GrContextForegroundSet(&sContext, ClrGreen);
    GrRectFill(&sContext, &sRect);

    drawOutline(130, 150, 10, 30);

    sRect.i16XMin = 170;
    sRect.i16YMin = 10;
    sRect.i16XMax = 190;
    sRect.i16YMax = 30;
    GrContextForegroundSet(&sContext, ClrBlue);
    GrRectFill(&sContext, &sRect);

    drawOutline(170, 190, 10, 30);
    
    sRect.i16XMin = 210;
    sRect.i16YMin = 10;
    sRect.i16XMax = 230;
    sRect.i16YMax = 30;
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectFill(&sContext, &sRect);

    drawOutline(210, 230, 10, 30);

    //Mazak
    GrContextFontSet(&sContext, g_psFontCm20b);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrStringDraw(&sContext, "CLEAR", -1, 240, 13 , 0);

    //Typy rysika 
    sRect.i16XMin = 10;
    sRect.i16YMin = 210;
    sRect.i16XMax = 30;
    sRect.i16YMax = 230;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);

    GrContextForegroundSet(&sContext, ClrBlack);
    GrCircleFill(&sContext, 50, 220, 10);

    //Rozmiar rysika 
    sRect.i16XMin = 80;
    sRect.i16YMin = 215;
    sRect.i16XMax = 110;
    sRect.i16YMax = 225;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);

    sRect.i16XMin = 120;
    sRect.i16YMin = 215;
    sRect.i16XMax = 150;
    sRect.i16YMax = 225;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);

    sRect.i16XMin = 130;
    sRect.i16YMin = 205;
    sRect.i16XMax = 140;
    sRect.i16YMax = 235;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
}
void drawBlankCanvas() {
    sRect.i16XMin = 0;
    sRect.i16YMin = 40;
    sRect.i16XMax = 320;
    sRect.i16YMax = 200;
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectFill(&sContext, &sRect);
}

void drawOutline(uint32_t xMin, uint32_t xMax, uint32_t yMin, uint32_t yMax) {
    //starczyloby GrRectDraw, ale nie dziala na emulatorze.. chyba...
    sRect.i16XMin = xMin - 5;
    sRect.i16YMin = yMin;
    sRect.i16XMax = xMax - 20;
    sRect.i16YMax = yMax;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);

    sRect.i16XMin = xMin + 20;
    sRect.i16YMin = yMin;
    sRect.i16XMax = xMax + 5;
    sRect.i16YMax = yMax;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);

    sRect.i16XMin = xMin - 5;
    sRect.i16YMin = yMin - 5;
    sRect.i16XMax = xMax + 5;
    sRect.i16YMax = yMax - 20;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);

    sRect.i16XMin = xMin - 5;
    sRect.i16YMin = yMin + 20;
    sRect.i16XMax = xMax + 5;
    sRect.i16YMax = yMax + 5;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
}

unsigned long penColor(unsigned long color, int32_t X, int32_t Y) {

    if((X > 210 && X < 230) && (Y > 0 && Y < 40)) {
        return ClrWhite;
    }

    if((X > 170 && X < 190) && (Y > 0 && Y < 40)) {
        return ClrBlue;
    }

    if((X > 130 && X < 150) && (Y > 0 && Y < 40)) {
        return ClrGreen;
    }

    if((X > 90 && X < 110) && (Y > 0 && Y < 40)) {
        return ClrYellow;
    }

    if((X > 50 && X < 70) && (Y > 0 && Y < 40)) {
        return ClrRed;
    }

    if((X > 10 && X < 30) && (Y > 0 && Y < 40)) {
        return ClrBlack;
    }

    return color;
}

bool penType(bool type, int32_t X, int32_t Y) {
    // true  - kwadrat, false - kolko
    if((X > 10 && X < 30) && (Y > 200 && Y < 240)) {
        return true;
    }
    
    if((X > 40 && X < 60) && (Y > 200 && Y < 240)) {
        return false;
    } 
    
    return type;
}

void penSize(int32_t X, int32_t Y) {
    if ((X > 80 && X < 110) && (Y > 200 && Y < 240)) {
        if(size > 1)
            size -= 1;
    }
    
    if ((X > 120 && X < 150) && (Y > 200 && Y < 240)) {
        if (size < 15)
            size += 1;
    }
}

int32_t TouchCallback(uint32_t ulMessage, int32_t lX, int32_t lY)
{
	color = penColor(color, lX, lY);
    type = penType(type, lX, lY);
    penSize(lX, lY);

    if(lY > 40 && lY < 200) {
        if(type) {
            sRect.i16XMin = lX - size;
            sRect.i16YMin = lY - size;
            sRect.i16XMax = lX + size;
            sRect.i16YMax = lY + size;
            GrContextForegroundSet(&sContext, color);
            GrRectFill(&sContext, &sRect);
        }
        else if(!type) {
            GrContextForegroundSet(&sContext, color);
            GrCircleFill(&sContext, lX, lY, size);
        }
        
    }

    if((lX > 240 && lX < 320) && (lY > 0 && lY < 40)) {
        drawMenu();
        drawBlankCanvas();
    }

	return 0;
}





int main(void)
{

    //
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    ROM_FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal.
    //

    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_8MHZ);

    //
    // Initialize the display driver.
    //
    ILI9341_240x320x262K_Init();

    //
    // Initialize the graphics context.
    //
    GrContextInit(&sContext, &g_sILI9341_240x320x262K);

    //Menu
    drawMenu();
    

    //Miejsce do rysowania
    drawBlankCanvas();


    //
    // Initialize touchscreen driver
    //
    TouchScreenInit();
    TouchScreenCalibrate(&sContext);
    TouchScreenCallbackSet(TouchCallback);

    GrContextFontSet(&sContext, g_psFontCm12);

    while(1)
    {

    }

}
