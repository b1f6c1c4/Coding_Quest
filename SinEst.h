#ifndef SINEST_H_
#define SINEST_H_
#include "IQmathLib.h"

#define SINEST_N 200

typedef struct
{
    Phasor_t Phasor;
    _iq20 Node[SINEST_N];
} SinEst;

void RunSinEst(SinEst *est, _iq20 data);

#endif
