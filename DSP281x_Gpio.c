// TI File $Revision: /main/2 $
// Checkin $Date: April 29, 2005   11:10:14 $
//###########################################################################
//
// FILE:    DSP281x_Gpio.c
//
// TITLE:   DSP281x General Purpose I/O Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP281x C/C++ Header Files V1.20 $
// $Release Date: July 27, 2009 $
//###########################################################################

#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

//---------------------------------------------------------------------------
// InitGpio:
//---------------------------------------------------------------------------
// This function initializes the Gpio to a known state.
//
void InitGpio(void)
{
    // Set GPIO A port pins,AL(Bits 7:0)(input)-AH(Bits 15:8) (output) 8bits
    // Input Qualifier = 0, none
    EALLOW;
    GpioMuxRegs.GPAMUX.all = 0x0340;
    // Enable CAP1-2; Enable EVA Timer1CMP; Disable PWM1-4 output.

    GpioMuxRegs.GPADIR.all    = 0x003F;
    GpioDataRegs.GPACLEAR.all = 0x003F;
    // GPIOA4:Short the current-limiting resistor? Now the resistor is Enabled.
    // GPIOA5:The Phase Indicator;
    // Assumption: setting 1 is dangerous (Disable the current-limiting resistor)
    // and enable PWM1-4;

    // GpioMuxRegs.GPFMUX.bit.SCIRXDA_GPIOF5=1;
    // GpioMuxRegs.GPFMUX.bit.SCITXDA_GPIOF4=0;
    EDIS;
}

//===========================================================================
// No more.
//===========================================================================
