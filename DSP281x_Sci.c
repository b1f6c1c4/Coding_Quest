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
    EALLOW;
    GpioMuxRegs.GPFMUX.bit.SCIRXDA_GPIOF5=1;
    GpioMuxRegs.GPFMUX.bit.SCITXDA_GPIOF4=0;
    EDIS;

    SciaRegs.SCICCR.bit.STOPBITS=0;        //1位停止位
    SciaRegs.SCICCR.bit.PARITY=1;			// Even parity
    SciaRegs.SCICCR.bit.PARITYENA=1;
    SciaRegs.SCICCR.bit.LOOPBKENA=0;       //禁止回送测试模式功能
    SciaRegs.SCICCR.bit.ADDRIDLE_MODE=0;   //空闲线模式
    SciaRegs.SCICCR.bit.SCICHAR=7;         //8位数据位

    SciaRegs.SCICTL1.bit.RXERRINTENA=0;
    SciaRegs.SCICTL1.bit.SLEEP=0;
    SciaRegs.SCICTL1.bit.RXENA=1;          //SCIA模块的接收使能

    SciaRegs.SCIHBAUD=0;
    SciaRegs.SCILBAUD=0x79;                //波特率为38400

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
