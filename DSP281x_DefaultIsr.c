#include "DSP281x_Device.h"    // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File


// Note CPU-Timer1 ISR is reserved for TI use.
interrupt void INT13_ISR(void)    // INT13 or CPU-Timer1
{
    asm ("     ESTOP0");
    for(;;);
}
// Note CPU-Timer2 ISR is reserved for TI use.
interrupt void INT14_ISR(void)    // CPU-Timer2
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void DATALOG_ISR(void)   // Datalogging interrupt
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void RTOSINT_ISR(void)   // RTOS interrupt
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void EMUINT_ISR(void)    // Emulation interrupt
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void NMI_ISR(void)      // Non-maskable interrupt
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void ILLEGAL_ISR(void)   // Illegal operation TRAP
{
  asm("        ESTOP0");
  for(;;);
}
interrupt void USER1_ISR(void)    // User Defined trap 1
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER2_ISR(void)    // User Defined trap 2
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER3_ISR(void)    // User Defined trap 3
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER4_ISR(void)    // User Defined trap 4
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER5_ISR(void)    // User Defined trap 5
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER6_ISR(void)    // User Defined trap 6
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER7_ISR(void)    // User Defined trap 7
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER8_ISR(void)    // User Defined trap 8
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER9_ISR(void)    // User Defined trap 9
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER10_ISR(void)    // User Defined trap 10
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER11_ISR(void)    // User Defined trap 11
{
    asm ("     ESTOP0");
    for(;;);
}
interrupt void USER12_ISR(void)    // User Defined trap 12
{
 // Insert ISR Code here
    asm ("     ESTOP0");
    for(;;);
}
// -----------------------------------------------------------
// PIE Group 1 - MUXed into CPU INT1
// -----------------------------------------------------------
// INT1.1
interrupt void PDPINTA_ISR( void )    // EV-A
{
    asm ("     ESTOP0");
    for(;;);
}
// INT1.2
interrupt void  PDPINTB_ISR(void)   // EV-B
{
    asm ("     ESTOP0");
    for(;;);
}
// INT1.3 - Reserved
// INT1.4
interrupt void  XINT1_ISR(void)
{
    asm ("     ESTOP0");
    for(;;);
}
// INT1.5
interrupt void  XINT2_ISR(void)
{
    asm ("     ESTOP0");
    for(;;);
}
// INT1.6
void Processing(int16 newACcurrent, int16 newDCvoltage);
interrupt void  ADCINT_ISR(void)    // ADC
{
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
    AdcRegs.ADCST.bit.INT_SEQ2_CLR = 1;
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
    AdcRegs.ADCTRL2.bit.RST_SEQ2 = 1;
    int16 ACcurrent;
    int16 DCvoltage;
    ACcurrent = (AdcRegs.ADCRESULT4>>4)+(AdcRegs.ADCRESULT5>>4)+(AdcRegs.ADCRESULT6>>4)
          +(AdcRegs.ADCRESULT7>>4)+(AdcRegs.ADCRESULT8>>4)+(AdcRegs.ADCRESULT9>>4)
          +(AdcRegs.ADCRESULT10>>4)+(AdcRegs.ADCRESULT11>>4);
    DCvoltage = (AdcRegs.ADCRESULT0>>4)+(AdcRegs.ADCRESULT1>>4)+(AdcRegs.ADCRESULT2>>4)
             +(AdcRegs.ADCRESULT3>>4)+(AdcRegs.ADCRESULT12>>4)+(AdcRegs.ADCRESULT13>>4)
             +(AdcRegs.ADCRESULT14>>4)+(AdcRegs.ADCRESULT15>>4);
    Processing(ACcurrent, DCvoltage);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
// INT1.7
interrupt void  TINT0_ISR(void)     // CPU-Timer 0
{
    asm ("     ESTOP0");
    for(;;);
}
// INT1.8
interrupt void  WAKEINT_ISR(void)    // WD
{
    asm ("     ESTOP0");
    for(;;);
}
// -----------------------------------------------------------
// PIE Group 2 - MUXed into CPU INT2
// -----------------------------------------------------------
// INT2.1
interrupt void CMP1INT_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
    asm ("     ESTOP0");
    for(;;);
}
// INT2.2
interrupt void CMP2INT_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
    asm ("     ESTOP0");
    for(;;);
}
// INT2.3
interrupt void CMP3INT_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
    asm ("     ESTOP0");
    for(;;);
}
// INT2.4
interrupt void T1PINT_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
    asm ("     ESTOP0");
    for(;;);
}
// INT2.5
interrupt void T1CINT_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
    asm ("     ESTOP0");
    for(;;);
}
// INT2.6
interrupt void T1UFINT_ISR(void)   // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
    asm ("     ESTOP0");
    for(;;);
}
// INT2.7
interrupt void T1OFINT_ISR(void)   // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
    asm ("     ESTOP0");
    for(;;);
}
// INT2.8 - Reserved
// -----------------------------------------------------------
// PIE Group 3 - MUXed into CPU INT3
// -----------------------------------------------------------
// INT 3.1
interrupt void T2PINT_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    asm ("     ESTOP0");
    for(;;);
}
// INT3.2
interrupt void T2CINT_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    asm ("     ESTOP0");
    for(;;);
}
// INT3.3
interrupt void T2UFINT_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    asm ("     ESTOP0");
    for(;;);
}
// INT3.4
interrupt void T2OFINT_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    asm ("     ESTOP0");
    for(;;);
}
// INT3.5
interrupt void CAPINT1_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    asm ("     ESTOP0");
    for(;;);
}
// INT3.6
interrupt void CAPINT2_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    asm ("     ESTOP0");
    for(;;);
}
// INT3.7
interrupt void CAPINT3_ISR(void)    // EV-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    asm ("     ESTOP0");
    for(;;);
}
// INT3.8 - Reserved
// -----------------------------------------------------------
// PIE Group 4 - MUXed into CPU INT4
// -----------------------------------------------------------
// INT 4.1
interrupt void CMP4INT_ISR(void)    // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    asm ("     ESTOP0");
    for(;;);
}
// INT4.2
interrupt void CMP5INT_ISR(void)    // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    asm ("     ESTOP0");
    for(;;);
}
// INT4.3
interrupt void CMP6INT_ISR(void)    // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    asm ("     ESTOP0");
    for(;;);
}
// INT4.4
interrupt void T3PINT_ISR(void)    // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    asm ("     ESTOP0");
    for(;;);
}
// INT4.5
interrupt void T3CINT_ISR(void)    // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    asm ("     ESTOP0");
    for(;;);
}
// INT4.6
interrupt void T3UFINT_ISR(void)    // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    asm ("     ESTOP0");
    for(;;);
}
// INT4.7
interrupt void T3OFINT_ISR(void)    // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
    asm ("     ESTOP0");
    for(;;);
}
// INT4.8 -- Reserved
// -----------------------------------------------------------
// PIE Group 5 - MUXed into CPU INT5
// -----------------------------------------------------------
// INT 5.1
interrupt void T4PINT_ISR(void)    // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
    asm ("     ESTOP0");
    for(;;);
}
// INT5.2
interrupt void T4CINT_ISR(void)    // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
    asm ("     ESTOP0");
    for(;;);
}
// INT5.3
interrupt void T4UFINT_ISR(void)   // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
    asm ("     ESTOP0");
    for(;;);
}
// INT5.4
interrupt void T4OFINT_ISR(void)   // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
    asm ("     ESTOP0");
    for(;;);
}
// INT5.5
interrupt void CAPINT4_ISR(void)   // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
    asm ("     ESTOP0");
    for(;;);
}
// INT5.6
interrupt void CAPINT5_ISR(void)   // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
    asm ("     ESTOP0");
    for(;;);
}
// INT5.7
interrupt void CAPINT6_ISR(void)   // EV-B
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
    asm ("     ESTOP0");
    for(;;);
}
// INT5.8 - Reserved
// -----------------------------------------------------------
// PIE Group 6 - MUXed into CPU INT6
// -----------------------------------------------------------
// INT6.1
interrupt void SPIRXINTA_ISR(void)    // SPI-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
    asm ("     ESTOP0");
    for(;;);
}
// INT6.2
interrupt void SPITXINTA_ISR(void)    // SPI-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
    asm ("     ESTOP0");
    for(;;);
}
// INT6.3 - Reserved
// INT6.4 - Reserved
// INT6.5
interrupt void MRINTA_ISR(void)    // McBSP-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
    asm ("     ESTOP0");
    for(;;);
}
// INT6.6
interrupt void MXINTA_ISR(void)    // McBSP-A
{
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
    asm ("     ESTOP0");
    for(;;);
}
// INT6.7 - Reserved
// INT6.8 - Reserved
// -----------------------------------------------------------
// PIE Group 7 - MUXed into CPU INT7
// -----------------------------------------------------------
// INT7.1 - Reserved
// INT7.2 - Reserved
// INT7.3 - Reserved
// INT7.4 - Reserved
// INT7.5 - Reserved
// INT7.6 - Reserved
// INT7.7 - Reserved
// INT7.8 - Reserved
// -----------------------------------------------------------
// PIE Group 8 - MUXed into CPU INT8
// -----------------------------------------------------------
// INT8.1 - Reserved
// INT8.2 - Reserved
// INT8.3 - Reserved
// INT8.4 - Reserved
// INT8.5 - Reserved
// INT8.6 - Reserved
// INT8.7 - Reserved
// INT8.8 - Reserved
// -----------------------------------------------------------
// PIE Group 9 - MUXed into CPU INT9
// -----------------------------------------------------------
// INT9.1
long SettingVoltage=250;
long DesiredPhaseDelay=0;
interrupt void SCIRXINTA_ISR(void)    // SCI-A
{
    char temp1,temp2,temp4;
    signed char temp3;
    temp1=SciaRegs.SCIRXBUF.all;
    temp2=SciaRegs.SCIRXBUF.all;
    temp3=SciaRegs.SCIRXBUF.all;
    temp4=SciaRegs.SCIRXBUF.all;
    if ((temp1==0x62) && (temp4==0x65))
    {
        SettingVoltage=((long) temp2)<<24;
        DesiredPhaseDelay=((long) temp3)<<24;
    }
    SciaRegs.SCIFFRX.bit.RXFIFORESET=0;//Reset the FIFO pointer to zero, and hold in reset.
    SciaRegs.SCIFFRX.bit.RXFIFORESET=1;//Re-enable receive FIFO operation
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
//   asm ("     ESTOP0");
//   for(;;);
}
// INT9.2
interrupt void SCITXINTA_ISR(void)    // SCI-A
{
    asm ("     ESTOP0");
    for(;;);
}
// INT9.3
interrupt void SCIRXINTB_ISR(void)    // SCI-B
{
    asm ("     ESTOP0");
    for(;;);
}
// INT9.4
interrupt void SCITXINTB_ISR(void)    // SCI-B
{
    asm ("     ESTOP0");
    for(;;);
}
// INT9.5
interrupt void ECAN0INTA_ISR(void)  // eCAN-A
{
    asm ("     ESTOP0");
    for(;;);
}
// INT9.6
interrupt void ECAN1INTA_ISR(void)  // eCAN-A
{
    asm ("     ESTOP0");
    for(;;);
}
// -----------------------------------------------------------
// PIE Group 10 - MUXed into CPU INT10
// -----------------------------------------------------------
// INT10.1 - Reserved
// INT10.2 - Reserved
// INT10.3 - Reserved
// INT10.4 - Reserved
// INT10.5 - Reserved
// INT10.6 - Reserved
// INT10.7 - Reserved
// INT10.8 - Reserved
// -----------------------------------------------------------
// PIE Group 11 - MUXed into CPU INT11
// -----------------------------------------------------------
// INT11.1 - Reserved
// INT11.2 - Reserved
// INT11.3 - Reserved
// INT11.4 - Reserved
// INT11.5 - Reserved
// INT11.6 - Reserved
// INT11.7 - Reserved
// INT11.8 - Reserved
// -----------------------------------------------------------
// PIE Group 12 - MUXed into CPU INT12
// -----------------------------------------------------------
// INT12.1 - Reserved
// INT12.2 - Reserved
// INT12.3 - Reserved
// INT12.4 - Reserved
// INT12.5 - Reserved
// INT12.6 - Reserved
// INT12.7 - Reserved
// INT12.8 - Reserved
//---------------------------------------------------------------------------
// Catch All Default ISRs:
//
interrupt void EMPTY_ISR(void)  // Empty ISR - only does a return.
{
}
interrupt void PIE_RESERVED(void)  // Reserved space.  For test.
{
  asm ("     ESTOP0");
  for(;;);
}
interrupt void rsvd_ISR(void)     // For test
{
  asm ("     ESTOP0");
  for(;;);
}
//===========================================================================
// No more.
//===========================================================================
