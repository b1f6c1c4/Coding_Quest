#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

void InitSci(void)
{
    SciaRegs.SCICTL1.bit.SWRESET = 0;

    // Initialize SCI-A:
    SciaRegs.SCICCR.bit.STOPBITS      = 0; // One stop bit
    SciaRegs.SCICCR.bit.PARITY        = 1; // Even parity
    SciaRegs.SCICCR.bit.PARITYENA     = 0; // Disable parity check
    SciaRegs.SCICCR.bit.LOOPBKENA     = 0; // Loop Back test mode disabled
    SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0; // Idle-line mode protocol selected
    SciaRegs.SCICCR.bit.SCICHAR       = 7; // Character Length:8

    SciaRegs.SCICTL1.bit.RXERRINTENA = 0; // Receive error interrupt disabled
    SciaRegs.SCICTL1.bit.SLEEP       = 0;
    SciaRegs.SCICTL1.bit.RXENA       = 1;
    // Send received characters to SCIRXEMU and SCIRXBUF

    SciaRegs.SCIHBAUD = 0;
    SciaRegs.SCILBAUD = 0x79; // baud rate is 38400

    SciaRegs.SCIFFRX.bit.RXFFOVRCLR  = 1; //  clear RXFFOVF flag
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 0; //  reset the FIFO pointer to zero, and hold in reset.
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 1; // Re-enable receive FIFO operation
    SciaRegs.SCIFFRX.bit.RXFFINTCLR  = 1; // clear RXFFINT flag
    SciaRegs.SCIFFRX.bit.RXFFIENA    = 1; // RX FIFO interrupt based on RXFFIVL match (less than or equal to) will be enabled.
    SciaRegs.SCIFFRX.bit.RXFFIL      = 1; // Receive FIFO generates interrupt when the FIFO status bits and FIFO level bits match (i.e., are greater than or equal to).

    SciaRegs.SCIPRI.bit.FREE = 0;
    SciaRegs.SCIPRI.bit.SOFT = 1;

    SciaRegs.SCICTL1.bit.SWRESET = 1;
}

long g_SettingVoltage    = 250;
long g_DesiredPhaseDelay = 0;

interrupt void SCIRXINTA_ISR(void)    // SCI-A
{
    char temp1,temp2,temp4;
    signed char temp3;

    temp1 = SciaRegs.SCIRXBUF.all;
    temp2 = SciaRegs.SCIRXBUF.all;
    temp3 = SciaRegs.SCIRXBUF.all;
    temp4 = SciaRegs.SCIRXBUF.all;

    if ((temp1 == 0x62) && (temp4 == 0x65))
    {
        g_SettingVoltage    = ((long) temp2) << 24;
        g_DesiredPhaseDelay = ((long) temp3) << 24;
    }

    SciaRegs.SCIFFRX.bit.RXFIFORESET = 0; // Reset the FIFO pointer to zero, and hold in reset.
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 1; // Re-enable receive FIFO operation

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}
