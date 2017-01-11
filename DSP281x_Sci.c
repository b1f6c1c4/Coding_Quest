#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File
#include "CtrlUnit.h"

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

    SciaRegs.SCICTL2.bit.RXBKINTENA  = 1; // Enable RX interrupt
    // Send received characters to SCIRXEMU and SCIRXBUF

    SciaRegs.SCIHBAUD = 0;
    SciaRegs.SCILBAUD = 0x79; // baud rate is 38400

    SciaRegs.SCIFFTX.bit.SCIFFENA  = 0; //  Disable FIFO mode
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

interrupt void SCIRXINTA_ISR(void)    // SCI-A
{
    char temp;
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;    // clear Receive interrupt flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
    temp = SciaRegs.SCIRXBUF.all;
    if (temp == 'x')
    {
        SetDanger();
    }
    else if (temp == 'c')
    {
        ClearDanger();
    }
    else if (temp == '6')
    {
        AdjPhaseDelay(1);
    }
    else if (temp == '4')
    {
        AdjPhaseDelay(-1);
    }
    else if (temp == '5')
    {
        AdjPhaseDelay(0);
    }
    else if (temp == '+')
    {
        AdjSetVol(1);
    }
    else if (temp == '-')
    {
        AdjSetVol(-1);
    }
    else if (temp == '0')
    {
        AdjSetVol(0);
    }
}
