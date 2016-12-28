// TI File $Revision: /main/2 $
// Checkin $Date: July 31, 2009   17:58:30 $
//###########################################################################
//
// FILE:   Example_281xAdc.c
//
// TITLE:  DSP281x ADC Example Program.
//
// ASSUMPTIONS:
//
//   This program requires the DSP281x V1.00 header files.  
//   As supplied, this project is configured for "boot to H0" operation.
//
//   Make sure the CPU clock speed is properly defined in 
//   DSP281x_Examples.h before compiling this example.
//
//   Connect signals to be converted to A2 and A3.
//  
//
// DESCRIPTION:
//
//   This example sets up the PLL in x10/2 mode, divides SYSCLKOUT    
//   by six to reach a 25Mhz HSPCLK (assuming a 30Mhz XCLKIN). The    
//   clock divider in the ADC is not used so that the ADC will see    
//   the 25Mhz on the HSPCLK. Interrupts are enabled and the EVA      
//   is setup to generate a periodic ADC SOC on SEQ1. Two channels    
//   are converted, ADCINA3 and ADCINA2.
//
//   Watch Variables:
// 
//         Voltage1[10]     Last 10 ADCRESULT0 values
//         Voltage2[10]     Last 10 ADCRESULT1 values
//         ConversionCount  Current result number 0-9
//         LoopCount        Idle loop counter  
//         
//
//###########################################################################
// $TI Release: DSP281x C/C++ Header Files V1.20 $
// $Release Date: July 27, 2009 $
//###########################################################################

#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

// Prototype statements for functions found within this file.
interrupt void adc_isr(void);

// Global variables used in this example:


main() 
{

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP281x_SysCtrl.c file.
   InitSysCtrl();

// For this example, set HSPCLK to SYSCLKOUT / 1 (150Mhz assuming 150Mhz SYSCLKOUT)
   EALLOW;
   SysCtrlRegs.HISPCP.all = 0x0;  // HSPCLK = SYSCLKOUT/1
   GpioMuxRegs.GPAMUX.all = 0x004F; // EVA Timer1CMP + PWM 1-4  pins
   EDIS;
   
// Step 2. Initialize GPIO: 
// This example function is found in the DSP281x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();  // Skipped for this example  

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
     
// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.       
   EALLOW;  // This is needed to write to EALLOW protected register
   PieVectTable.ADCINT = &adc_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers
   init_eva();
// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP281x_InitPeripherals.c
// InitPeripherals(); // Not required for this example
   InitAdc();  // For this example, init the ADC

// Step 5. User specific code, enable interrupts:

// Enable ADCINT in PIE
   PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
   IER |= M_INT1; // Enable CPU Interrupt 1
   EINT;          // Enable Global interrupt INTM
   ERTM;          // Enable Global realtime interrupt DBGM

   LoopCount = 0;
   ConversionCount = 0;
    
// Configure ADC
   AdcRegs.ADCTRL1.bit.RESET=0;		//ADC module software reset
   AdcRegs.ADCTRL1.bit.SUSMOD=1;		//Emulation-suspend mode
   AdcRegs.ADCTRL1.bit.ACQ_PS=4;		//Sampling Window (ACQ_PS-1)*ADCLK
   AdcRegs.ADCTRL1.bit.CPS=1;			//Core clock pre-scaler
   AdcRegs.ADCTRL1.bit.CONT_RUN=0;		//Continuous run
   AdcRegs.ADCTRL1.bit.SEQ_OVRD=0;		//Sequencer override
   AdcRegs.ADCTRL1.bit.SEQ_CASC=1;		//Cascaded sequencer operation
   //SEQ1 and SEQ2 operate as a single 16-state sequencer   (SEQ)

   AdcRegs.ADCTRL2.bit.EVB_SOC_SEQ=0;	//EVB SOC enable for cascaded sequencer
   AdcRegs.ADCTRL2.bit.RST_SEQ1=1;		//Immediately reset sequencer to state CONV00
   AdcRegs.ADCTRL2.bit.SOC_SEQ1=0;		//Clears a pending SOC trigger
   AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1=1;	//Interrupt request by INT SEQ1 is enabled.
   AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1=0;	//INT SEQ1 is set at the end of every SEQ1 sequence.
   AdcRegs.ADCTRL2.bit.EVA_SOC_SEQ1=1;	//Allows SEQ1/SEQ to be started by Event Manager A trigger.
   AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1=0;	//External signal start-of-conversion bit for SEQ1
   AdcRegs.ADCTRL2.bit.RST_SEQ2=1;		//Immediately resets SEQ2 to an initial ¡°pretriggered¡± state
   AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2=1;	//Interrupt request by INT SEQ2 is enabled
   AdcRegs.ADCTRL2.bit.INT_MOD_SEQ2=0;	//INT SEQ2 is set at the end of every SEQ2 sequence.
   AdcRegs.ADCTRL2.bit.EVB_SOC_SEQ2=0;	//SEQ2 cannot be started by EVB trigger.

   AdcRegs.ADCTRL3.bit.ADCEXTREF=0;	//ADCREFP(2V) and ADCREFM(1V) pins are outputs for internal reference sources.
   AdcRegs.ADCTRL3.bit.ADCCLKPS=4;		//Core clock divider	//9.375MHz
   AdcRegs.ADCTRL3.bit.SMODE_SEL=0;	//Sequential sampling mode is selected.
   AdcRegs.ADCMAXCONV.all=0x0F;
   AdcRegs.ADCCHSELSEQ1.bit.CONV00=0x0;	// Ref_1
   AdcRegs.ADCCHSELSEQ1.bit.CONV01=0x1;	// Ref_2
   AdcRegs.ADCCHSELSEQ1.bit.CONV02=0x2;	// DC voltage
   AdcRegs.ADCCHSELSEQ1.bit.CONV03=0x2;	// DC voltage
   AdcRegs.ADCCHSELSEQ2.bit.CONV04=0x3;	// AC voltage
   AdcRegs.ADCCHSELSEQ2.bit.CONV05=0x3;	// AC voltage
   AdcRegs.ADCCHSELSEQ2.bit.CONV06=0x4;	// AC current
   AdcRegs.ADCCHSELSEQ2.bit.CONV07=0x4;	// AC current
   AdcRegs.ADCCHSELSEQ3.bit.CONV08=0x4;	// AC current
   AdcRegs.ADCCHSELSEQ3.bit.CONV09=0x4;	// AC current
   AdcRegs.ADCCHSELSEQ3.bit.CONV10=0x3;	// AC voltage
   AdcRegs.ADCCHSELSEQ3.bit.CONV11=0x3;	// AC voltage
   AdcRegs.ADCCHSELSEQ4.bit.CONV12=0x2;	// DC voltage
   AdcRegs.ADCCHSELSEQ4.bit.CONV13=0x2;	// DC voltage
   AdcRegs.ADCCHSELSEQ4.bit.CONV14=0x1;	// Ref_2
   AdcRegs.ADCCHSELSEQ4.bit.CONV15=0x0;	// Ref_1

// Wait for ADC interrupt
   while(1)
   {
      LoopCount++;
   }

}


interrupt void  adc_isr(void)
{

  EvaRegs.CMPR1 = AdcRegs.ADCRESULT0;
  EvaRegs.CMPR2 = AdcRegs.ADCRESULT0;

  // Reinitialize for next ADC sequence
  AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
  AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
  
  return;
}
