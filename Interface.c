#include "Interface.h"
#include "Core.h"

#include "DSP281x_Device.h"
#include "DSP281x_Examples.h"

#define UAC_COEF _IQ20(310.655)
#define UAC_OFFSET 36055
#define IAC_COEF _IQ20(7.33496)
#define IAC_OFFSET 34937
#define UDC_COEF _IQ20(410.779)
#define UDC_OFFSET _IQ20(4.4475)

void IF_SetPwm(_iq20 level)
{
    unsigned long cmpr = _IQ15mpyIQX((level+_IQ20(1.0)), 20, _IQ18(3750), 18) >> 15;
    EvaRegs.CMPR1 = cmpr;
    EvaRegs.CMPR2 = cmpr;

    EALLOW;
    GpioMuxRegs.GPAMUX.all  = 0x031F;
    GpioMuxRegs.GPADIR.all  = 0x006F;
    GpioDataRegs.GPASET.all = 0x0040;
    EDIS;
}

void IF_Off()
{
    EALLOW;
    GpioMuxRegs.GPAMUX.all    = 0x0310;
    GpioMuxRegs.GPADIR.all    = 0x006F;
    GpioDataRegs.GPACLEAR.all = 0x004F;
    EDIS;
}

void RawProcess(long uAC, long iAC, long uDC)
{
    EALLOW;
    GpioDataRegs.GPATOGGLE.all = 1<<5;
    EDIS;

    Process(
        _IQ20mpyIQX(uAC - UAC_OFFSET, 16, UAC_COEF, 20),
        _IQ20mpyIQX(iAC - IAC_OFFSET, 16, IAC_COEF, 20),
        _IQ20mpyIQX(uDC, 16, UDC_COEF, 20) - UDC_OFFSET);
}
