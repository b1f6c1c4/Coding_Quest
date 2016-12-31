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

long g_SettingVoltage=250;
long g_DesiredPhaseDelay=0;

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
        g_SettingVoltage=((long) temp2)<<24;
        g_DesiredPhaseDelay=((long) temp3)<<24;
    }
    SciaRegs.SCIFFRX.bit.RXFIFORESET=0;//Reset the FIFO pointer to zero, and hold in reset.
    SciaRegs.SCIFFRX.bit.RXFIFORESET=1;//Re-enable receive FIFO operation
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
//   asm ("     ESTOP0");
//   for(;;);
}
