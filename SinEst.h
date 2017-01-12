#ifndef SINEST_H_
#define SINEST_H_
#include "IQmathLib.h"

#define SINEST_N 200

typedef struct
{
    _iq20 Rms;
    _iq20 Phase;
    _iq20 Node[SINEST_N];
} SinEst;

extern SinEst g_ACvoltage;
extern SinEst g_ACcurrent;

void RunSinEst(SinEst *est, _iq20 data);

#endif /* SINEST_H_ */
