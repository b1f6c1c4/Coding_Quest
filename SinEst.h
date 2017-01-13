#ifndef SINEST_H_
#define SINEST_H_

#include "IQmathLib.h"

#define SINEST_N 200

typedef struct
{
    _iq20 Node[SINEST_N];
} SinEst;

Phasor_t RunSinEst(SinEst *est, _iq20 data);

#endif
