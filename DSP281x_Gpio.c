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
    GpioMuxRegs.GPAMUX.all = 0x0310;
    // Enable CAP1-2; Enable PWM5; Disable PWM1-4 output.;

    GpioMuxRegs.GPADIR.all    = 0x006F;
    GpioDataRegs.GPACLEAR.all = 0x006F;
    // GPIO
    // GPIOA4: PWM5 I_Ref PWM output
    // GPIOA5: The Phase Indicator;
    // GPIOA6: Control the current-limiting resistor;
    // Low: resistor connected in. High resistro shorted.
    // Assumption: setting 1 is dangerous (Short the current-limiting resistor)

    // GpioMuxRegs.GPFMUX.bit.SCIRXDA_GPIOF5=1;
    // GpioMuxRegs.GPFMUX.bit.SCITXDA_GPIOF4=0;
    EDIS;
}

//===========================================================================
// No more.
//===========================================================================
