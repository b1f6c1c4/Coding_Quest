#ifndef SINEST_H_
#define SINEST_H_
#include "Phasor.h"
#include "IQmathLib.h"

#define SINEST_N 200

typedef struct
{
    _iq20 Node[SINEST_N];
} SinEst_t;

Phasor_t Sin_Run(SinEst_t *est, _iq20 data);

#endif
