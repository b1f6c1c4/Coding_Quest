#include "DSP281x_Device.h"    // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

#define ADC_usDELAY  8000L
#define ADC_usDELAY2 20L
extern void DSP28x_usDelay(Uint32 Count);

#define DC_VOLT 0x5
#define AC_CURR 0x0
#define AC_VOLT 0x1
//0x2,0x4 won't work
// This function initializes ADC to a known state.
void InitAdc(void)
{


    // To powerup the ADC the ADCENCLK bit should be set first to enable
    // clocks, followed by powering up the bandgap and reference circuitry.
    // After a 5ms delay the rest of the ADC can be powered up. After ADC
    // powerup, another 20us delay is required before performing the first
    // ADC conversion. Please note that for the delay function below to
    // operate correctly the CPU_CLOCK_SPEED define statement in the
    // DSP28_Examples.h file must contain the correct CPU clock period in
    // nanoseconds. For example:

    AdcRegs.ADCTRL3.bit.ADCBGRFDN = 0x3; // Power up bandgap/reference circuitry
    DELAY_US(ADC_usDELAY);               // Delay before powering up rest of ADC
    AdcRegs.ADCTRL3.bit.ADCPWDN = 1;     // Power up rest of ADC
    DELAY_US(ADC_usDELAY2);              // Delay after powering up ADC

    // Configure ADC
    AdcRegs.ADCTRL1.bit.RESET    = 0; // ADC module software reset
    AdcRegs.ADCTRL1.bit.SUSMOD   = 1; // Emulation-suspend mode
    AdcRegs.ADCTRL1.bit.ACQ_PS   = 7; // Sampling Window (ACQ_PS+1)*ADCLK
    AdcRegs.ADCTRL1.bit.CPS      = 0; // Core clock pre-scaler
    AdcRegs.ADCTRL1.bit.CONT_RUN = 0; // Continuous run
    AdcRegs.ADCTRL1.bit.SEQ_OVRD = 0; // Sequencer override
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1; // Cascaded sequencer operation
    // SEQ1 and SEQ2 operate as a single 16-state sequencer   (SEQ)

    AdcRegs.ADCTRL2.bit.EVB_SOC_SEQ  = 0; // EVB SOC enable for cascaded sequencer
    AdcRegs.ADCTRL2.bit.RST_SEQ1     = 1; // Immediately reset sequencer to state CONV00
    AdcRegs.ADCTRL2.bit.SOC_SEQ1     = 0; // Clears a pending SOC trigger
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1; // Interrupt request by INT SEQ1 is enabled.
    AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 0; // INT SEQ1 is set at the end of every SEQ1 sequence.
    AdcRegs.ADCTRL2.bit.EVA_SOC_SEQ1 = 1; // Allows SEQ1/SEQ to be started by Event Manager A trigger.
    AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1 = 0; // External signal start-of-conversion bit for SEQ1
    AdcRegs.ADCTRL2.bit.RST_SEQ2     = 1; // Immediately resets SEQ2 to an initial "pretriggered" state
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2 = 1; // Interrupt request by INT SEQ2 is enabled
    AdcRegs.ADCTRL2.bit.INT_MOD_SEQ2 = 0; // INT SEQ2 is set at the end of every SEQ2 sequence.
    AdcRegs.ADCTRL2.bit.EVB_SOC_SEQ2 = 0; // SEQ2 cannot be started by EVB trigger.

    AdcRegs.ADCTRL3.bit.ADCEXTREF = 0;   // ADCREFP(2V) and ADCREFM(1V) pins are outputs for internal reference sources.
    AdcRegs.ADCTRL3.bit.ADCCLKPS  = 0xF; // Core clock divider: 30. ADCCLK works at 5MHz.
    AdcRegs.ADCTRL3.bit.SMODE_SEL = 0;   // Sequential sampling mode is selected.
    AdcRegs.ADCMAXCONV.all        = 0x0F;

    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = DC_VOLT;
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = DC_VOLT;
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = AC_VOLT;
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = AC_VOLT;
    AdcRegs.ADCCHSELSEQ2.bit.CONV04 = AC_CURR;
    AdcRegs.ADCCHSELSEQ2.bit.CONV05 = AC_CURR;
    AdcRegs.ADCCHSELSEQ2.bit.CONV06 = AC_CURR;
    AdcRegs.ADCCHSELSEQ2.bit.CONV07 = AC_CURR;
    AdcRegs.ADCCHSELSEQ3.bit.CONV08 = AC_CURR;
    AdcRegs.ADCCHSELSEQ3.bit.CONV09 = AC_CURR;
    AdcRegs.ADCCHSELSEQ3.bit.CONV10 = AC_CURR;
    AdcRegs.ADCCHSELSEQ3.bit.CONV11 = AC_CURR;
    AdcRegs.ADCCHSELSEQ4.bit.CONV12 = AC_VOLT;
    AdcRegs.ADCCHSELSEQ4.bit.CONV13 = AC_VOLT;
    AdcRegs.ADCCHSELSEQ4.bit.CONV14 = DC_VOLT;
    AdcRegs.ADCCHSELSEQ4.bit.CONV15 = DC_VOLT;
}

void Processing(int16, int16);
interrupt void  ADCINT_ISR(void)    // ADC
{
    int16 ACcurrent = 0;
    int16 DCvoltage = 0;
    int16 ACvoltage = 0;

    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
    AdcRegs.ADCST.bit.INT_SEQ2_CLR = 1;
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
    AdcRegs.ADCTRL2.bit.RST_SEQ2 = 1;

    DCvoltage += AdcRegs.ADCRESULT0  >> 4;
    DCvoltage += AdcRegs.ADCRESULT1  >> 4;
    ACvoltage += AdcRegs.ADCRESULT2  >> 4;
    ACvoltage += AdcRegs.ADCRESULT3  >> 4;
    ACcurrent += AdcRegs.ADCRESULT4  >> 4;
    ACcurrent += AdcRegs.ADCRESULT5  >> 4;
    ACcurrent += AdcRegs.ADCRESULT6  >> 4;
    ACcurrent += AdcRegs.ADCRESULT7  >> 4;
    ACcurrent += AdcRegs.ADCRESULT8  >> 4;
    ACcurrent += AdcRegs.ADCRESULT9  >> 4;
    ACcurrent += AdcRegs.ADCRESULT10 >> 4;
    ACcurrent += AdcRegs.ADCRESULT11 >> 4;
    ACvoltage += AdcRegs.ADCRESULT12 >> 4;
    ACvoltage += AdcRegs.ADCRESULT13 >> 4;
    DCvoltage += AdcRegs.ADCRESULT14 >> 4;
    DCvoltage += AdcRegs.ADCRESULT15 >> 4;

    Processing(ACcurrent, DCvoltage);

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
