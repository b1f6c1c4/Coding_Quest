#include "DSP281x_Device.h" //  DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

//---------------------------------------------------------------------------
// InitPeripherals:
//---------------------------------------------------------------------------
// The following function initializes the peripherals to a default state.
// It calls each of the peripherals default initialization functions.
// This function should be executed at boot time or on a soft reset.
//
void InitPeripherals(void)
{

//  Initialize CPU Timers To default State:
    InitCpuTimers();

//  Initialize Event Manager Peripheral To default State:
    InitEv();
    
//  Initialize ADC Peripheral To default State:
    InitAdc();
    
//  Initialize eCAN Peripheral To default State:
    //InitECan();

//  Initialize SPI Peripherals To default State:
    //InitSpi();

//  Initialize SCI Peripherals To default State:
    InitSci();
}

//===========================================================================
// No more.
//===========================================================================
