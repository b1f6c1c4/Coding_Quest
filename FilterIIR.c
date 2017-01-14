#include "FilterIIR.h"

_iq20 IIR_Run(IIR_t *iir, _iq20 val)
{
    _iq20 temp1, temp2;

    temp1  = _IQ20mpyIQX(iir->Gain    , 30, val       , 20);
    temp1 -= _IQ20mpyIQX(iir->Denom[0], 30, iir->Node1, 20);
    temp1 -= _IQ20mpyIQX(iir->Denom[1], 30, iir->Node2, 20);
    temp2  = temp1;
    temp2 += _IQ20mpyIQX(iir->Numer[0], 30, iir->Node1, 20);
    temp2 += _IQ20mpyIQX(iir->Numer[1], 30, iir->Node2, 20);

    iir->Node2 = iir->Node1;
    iir->Node1 = temp1;
    return temp2;
}

_iq20 IIR_RunN(IIR_t *iir, int n, _iq20 val)
{
    while (n-- > 0)
        val = IIR_Run(iir++, val);
    return val;
}
