#include "Interface.h"
#include "Core.h"

#include "DSP281x_Device.h"
#include "DSP281x_Examples.h"

// TODO
#define UAC_COEF _IQ20(0.0)
#define UAC_OFFSET 0
#define IAC_COEF _IQ20(0.0)
#define IAC_OFFSET 0
#define UDC_COEF _IQ20(0.0)
#define UDC_OFFSET 0

void IF_SetPwm(_iq20 level)
{
    unsigned long cmpr = _IQ15mpyIQX((level+_IQ20(1.0)), 20, _IQ18(3750), 18) >> 15;
    EvaRegs.CMPR1 = cmpr + cmproffset;
    EvaRegs.CMPR2 = cmpr + cmproffset;

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
    Process(
        _IQ20rmpy(uAC - UAC_OFFSET, UAC_COEF),
        _IQ20rmpy(iAC - IAC_OFFSET, IAC_COEF),
        _IQ20rmpy(uDC - UDC_OFFSET, UDC_COEF));
}
