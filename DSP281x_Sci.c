#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File
#include "Core.h"

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
    SciaRegs.SCICTL1.bit.TXENA       = 0; // Transmitter disabled
    SciaRegs.SCICTL1.bit.RXENA       = 1;

    SciaRegs.SCICTL2.bit.RXBKINTENA  = 1; // Enable RX interrupt
    // Send received characters to SCIRXEMU and SCIRXBUF
    SciaRegs.SCICTL2.bit.TXINTENA    = 1; // Enable TX interrupt

    SciaRegs.SCIHBAUD = 0;
    SciaRegs.SCILBAUD = 0x79; // baud rate is 38400

    SciaRegs.SCIFFTX.bit.SCIFFENA    = 1; // Enable FIFO mode
    SciaRegs.SCIFFTX.bit.SCIRST      = 0; // Reset the SCI transmit and receive channels
    SciaRegs.SCIFFTX.bit.TXFIFOXRESET= 0; // Reset the FIFO pointer to zero and hold in reset
    SciaRegs.SCIFFTX.bit.TXINTCLR    = 1; // Clear TXFFINT flag
    SciaRegs.SCIFFTX.bit.TXFFIENA    = 1; // TX FIFO interrupt based on TXFFIVL match (less than or equal to) is enabled.
    SciaRegs.SCIFFTX.bit.TXFFILIL    = 0; // Transmit FIFO generates interrupt when the FIFO status bits and FIFO level bits match (less than or equal to).

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
    switch (temp)
    {
        case 'x':
            ChangeState(S_IDLE);
            break;
        case 'm':
            ChangeState(S_IMP);
            break;
        case 'c':
            ChangeState(S_CURR);
            break;
        case 'f':
            ChangeState(S_FULL);
            break;
        case '1':
            if (g_State != S_FULL)
                g_TargetACcurrent.Im -= _IQ20(0.010);
            break;
        case '2':
            if (g_State != S_FULL)
                g_TargetACcurrent.Im = 0;
            break;
        case '3':
            if (g_State != S_FULL)
                g_TargetACcurrent.Im += _IQ20(0.010);
            break;
        case '4':
            if (g_State != S_FULL)
                g_TargetACcurrent.Re -= _IQ20(0.010);
            break;
        case '5':
            if (g_State != S_FULL)
                g_TargetACcurrent.Re = 0;
            break;
        case '6':
            if (g_State != S_FULL)
                g_TargetACcurrent.Re += _IQ20(0.010);
            break;
        case '7':
            g_TargetDCvoltage -= _IQ20(5);
            if (g_TargetDCvoltage < 0)
                g_TargetDCvoltage = 0;
            break;
        case '8':
            g_TargetDCvoltage = 0;
            break;
        case '9':
            g_TargetDCvoltage += _IQ20(5);
            break;
        case 'q':
            g_TargetTanPhi -= _IQ20(0.01);
            if (g_TargetTanPhi < TARG_MIN_TANPHI)
                g_TargetTanPhi = TARG_MIN_TANPHI;
            break;
        case 'w':
            g_TargetTanPhi = 0;
            break;
        case 'e':
            g_TargetTanPhi += _IQ20(0.01);
            if (g_TargetTanPhi > TARG_MAX_TANPHI)
                g_TargetTanPhi = TARG_MAX_TANPHI;
            break;
    }
}

interrupt void SCITXINTA_ISR(void)    // SCI-A
{
    SciaRegs.SCIFFTX.bit.TXINTCLR    = 1; // Remember to clear TXFFINT flag
    SciaRegs.SCICTL1.bit.TXENA       = 0; // Transmitter disabled
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

void UartSend(unsigned long DataToSend)
{
    SciaRegs.SCICTL1.bit.TXENA = 1;     // When TXENA == 0, SCITXBUF can not be written
    SciaRegs.SCITXBUF = 0x00;
    SciaRegs.SCITXBUF = DataToSend>>24;
    SciaRegs.SCITXBUF = DataToSend>>16;
    SciaRegs.SCITXBUF = DataToSend>>8;
    SciaRegs.SCITXBUF = DataToSend;
    SciaRegs.SCITXBUF = (DataToSend>>24) + (DataToSend>>16) + (DataToSend>>8) + DataToSend;
}

