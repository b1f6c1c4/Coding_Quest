#ifndef FILTERIIR_H_
#define FILTERIIR_H_

#include "IQmathLib.h"

typedef struct
{
    _iq30 Numer[2];
    _iq30 Denom[2];
    _iq30 Gain;

    _iq20 Node1;
    _iq20 Node2;
} IIR_t;

_iq20 IIR_Run(IIR_t *iir, _iq20 val);
_iq20 IIR_RunN(IIR_t *iir, int n, _iq20 val);

#endif
