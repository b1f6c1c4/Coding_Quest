#include "Core.h"

#include "DSP281x_Device.h"
#include "DSP281x_Examples.h"

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
