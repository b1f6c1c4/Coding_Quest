#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File
#include "CtrlUnit.h"

void InitSci(void)
{
    SciaRegs.SCICTL1.bit.SWRESET = 0;

    // Initialize SCI-A:
    SciaRegs.SCICCR.bit.STOPBITS      = 0; // One stop bit
    SciaRegs.SCICCR.bit.PARITY        = 0; // Odd parity
    SciaRegs.SCICCR.bit.PARITYENA     = 0; // Disable parity check
    SciaRegs.SCICCR.bit.LOOPBKENA     = 0; // Loop Back test mode disabled
    SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0; // Idle-line mode protocol selected
    SciaRegs.SCICCR.bit.SCICHAR       = 7; // Character Length:8

    SciaRegs.SCICTL1.bit.RXERRINTENA = 0; // Receive error interrupt disabled
    SciaRegs.SCICTL1.bit.TXWAKE      = 0; // Transmit feature is not selected
    SciaRegs.SCICTL1.bit.SLEEP       = 0;
    SciaRegs.SCICTL1.bit.TXENA       = 1; // Transmitter enabled
    SciaRegs.SCICTL1.bit.RXENA       = 1;

    SciaRegs.SCICTL2.bit.RXBKINTENA  = 1; // Enable RX interrupt
    // Send received characters to SCIRXEMU and SCIRXBUF
    SciaRegs.SCICTL2.bit.TXINTENA    = 1; // Enable TX interrupt

    SciaRegs.SCIHBAUD = 0;
    SciaRegs.SCILBAUD = 0x79; // baud rate is 38400

    SciaRegs.SCIFFTX.bit.SCIRST      = 0; // Reset the SCI transmit and receive channels
    SciaRegs.SCIFFTX.bit.SCIFFENA    = 1; // Enable FIFO mode
    SciaRegs.SCIFFTX.bit.TXFIFOXRESET= 0; // Reset the FIFO pointer to zero and hold in reset
    SciaRegs.SCIFFTX.bit.TXINTCLR    = 1; // Clear TXFFINT flag
    SciaRegs.SCIFFTX.bit.TXFFIENA    = 1; // TX FIFO interrupt based on TXFFIVL match (less than or equal to) is enabled.
    SciaRegs.SCIFFTX.bit.TXFFILIL    = 10; // Transmit FIFO generates interrupt when the FIFO status bits and FIFO level bits match (less than or equal to).

    SciaRegs.SCIFFRX.bit.RXFFOVRCLR  = 1; // Clear RXFFOVF flag
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 0; // Reset the FIFO pointer to zero, and hold in reset.
    SciaRegs.SCIFFRX.bit.RXFFINTCLR  = 1; // Clear RXFFINT flag
    SciaRegs.SCIFFRX.bit.RXFFIENA    = 1; // RX FIFO interrupt based on RXFFIVL match (less than or equal to) will be enabled.
    SciaRegs.SCIFFRX.bit.RXFFIL      = 1; // Receive FIFO generates interrupt when the FIFO status bits and FIFO level bits match (i.e., are greater than or equal to).

    SciaRegs.SCIFFCT.bit.FFTXDLY     = 1; // FIFO transfer delay is 1 Baud-Clock

    SciaRegs.SCIPRI.bit.FREE = 0;
    SciaRegs.SCIPRI.bit.SOFT = 1;

    SciaRegs.SCIFFTX.bit.TXFIFOXRESET= 1; // Re-enable transmit FIFO operation
    SciaRegs.SCIFFRX.bit.RXFIFORESET = 1; // Re-enable receive FIFO operation
    SciaRegs.SCIFFTX.bit.SCIRST    = 1; // SCI FIFO can resume transmit or receive
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
        AdjPhaseDelay(-1);
    }
    else if (temp == '4')
    {
        AdjPhaseDelay(1);
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
    else if (temp == '3')
    {
        AdjACRef(1);
        EALLOW;
        GpioDataRegs.GPATOGGLE.all=0x0040;
        EDIS;
    }
    else if (temp == '1')
    {
        AdjACRef(-1);
        EALLOW;
        GpioDataRegs.GPATOGGLE.all=0x0040;
        EDIS;
    }
    else if (temp == '2')
    {
        AdjACRef(0);
    }
    else if (temp == 'e')
    {
        AdjACKp(1);
    }
    else if (temp == 'q')
    {
        AdjACKp(-1);
    }
    else if (temp == 'w')
    {
        AdjACKp(0);
    }
    else if (temp == 'd')
    {
        AdjACKi(1);
    }
    else if (temp == 'a')
    {
        AdjACKi(-1);
    }
    else if (temp == 's')
    {
        AdjACKi(0);
    }
    else if (temp == '9')
    {
        Adjcmproffset(1);
    }
    else if (temp == '7')
    {
        Adjcmproffset(-1);
    }
    else if (temp == '8')
    {
        Adjcmproffset(0);
    }
    else if (temp == 'z')
    {
        OpenLoopCtrl();
    }
    else if (temp == 'v')
    {
        CloseLoopCtrl();
    }
}

interrupt void SCITXINTA_ISR(void)    // SCI-A
{
    SciaRegs.SCIFFTX.bit.TXINTCLR    = 1; // Remember to clear TXFFINT flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
    if(SendFlag)
    {
        SciaRegs.SCITXBUF = 0x00;
        SciaRegs.SCITXBUF = DataToSend>>24;
        SciaRegs.SCITXBUF = DataToSend>>16;
        SciaRegs.SCITXBUF = DataToSend>>8;
        SciaRegs.SCITXBUF = DataToSend;
        SciaRegs.SCITXBUF = DataToSend>>24 +
                DataToSend>>16 + DataToSend>>8 + DataToSend;
    }
}
