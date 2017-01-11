#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

//  Prototype statements for functions found within this file:

//  Global variables used in this example:


main()
{

    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the DSP281x_SysCtrl.c file.
    InitSysCtrl();

    // For this example, set HSPCLK to SYSCLKOUT / 1 (150Mhz assuming 150Mhz SYSCLKOUT)

    // Step 2. Initialize GPIO:
    // This example function is found in the DSP281x_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    InitGpio();

    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    DINT;

    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the DSP281x_PieCtrl.c file.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in DSP281x_DefaultIsr.c.
    // This function is found in DSP281x_PieVect.c.
    InitPieVectTable();

    // Step 4. Initialize all the Device Peripherals:
    // This function is found in DSP281x_InitPeripherals.c
    InitEv(); // For this example, init the EV

    /*  Assumptions: In the process of turning bipolar input to uni-polar input, the Vref must be used as ADCGND.
     *   Still, you shold use bipolar input conversion reference as calibration input. When converting bipolar to
     *   unipolar signals, make sure that the reference voltage used as the mid-point of the ADC range is fed as
     *   an input calibration channel. This removes any bipolar offset error as discussed earlier.
     */
    InitAdc();  // For this example, init the ADC

    InitSci();

    // Step 5. User specific code, enable interrupts:
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1; // Enable ADCINT in PIE
    PieCtrlRegs.PIEIER3.bit.INTx5 = 1; // Enable CAP1
    PieCtrlRegs.PIEIER3.bit.INTx6 = 1; // Enable CAP2
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1; //
    IER |= M_INT1+M_INT3+M_INT9;              // Enable CPU Interrupt 1
    EINT;                              // Enable Global interrupt INTM
    ERTM;                              // Enable Global realtime interrupt DBGM

    //  Wait for ADC interrupt
    while (1) { }
}
