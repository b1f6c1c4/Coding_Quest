#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

//---------------------------------------------------------------------------
// InitSPI:
//---------------------------------------------------------------------------
// This function initializes the SPI(s) to a known state.
//
void InitSci(void)
{
    // Initialize SCI-A:
    SciaRegs.SCICCR.bit.STOPBITS=0;     //One stop bit
    SciaRegs.SCICCR.bit.PARITY=1;       //Even parity
    SciaRegs.SCICCR.bit.PARITYENA=1;
    SciaRegs.SCICCR.bit.LOOPBKENA=0;    //Loop Back test mode disabled
    SciaRegs.SCICCR.bit.ADDRIDLE_MODE=0;//Idle-line mode protocol selected
    SciaRegs.SCICCR.bit.SCICHAR=7;      //Character Length:8

    SciaRegs.SCICTL1.bit.RXERRINTENA=0; //Receive error interrupt disabled
    SciaRegs.SCICTL1.bit.SLEEP=0;
    SciaRegs.SCICTL1.bit.RXENA=1;
    //Send received characters to SCIRXEMU and SCIRXBUF

    SciaRegs.SCIHBAUD=0;
    SciaRegs.SCILBAUD=0x79;             //²¨ÌØÂÊÎª38400

    SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;
    SciaRegs.SCIFFRX.bit.RXFIFORESET=0;
    SciaRegs.SCIFFRX.bit.RXFIFORESET=1;
    SciaRegs.SCIFFRX.bit.RXFFINTCLR=1;
    SciaRegs.SCIFFRX.bit.RXFFIENA=1;
    SciaRegs.SCIFFRX.bit.RXFFIL=4;

    SciaRegs.SCIPRI.bit.FREE=0;
    SciaRegs.SCIPRI.bit.SOFT=1;

    SciaRegs.SCICTL1.bit.SWRESET=1;
}
