#ifndef PI_H_
#define PI_H_

#include "FixedPoint.h"
#include "Phasor.h"

typedef struct
{
    _iq20 PosSat;
    _iq20 NegSat;
    _iq20 Kp;
    _iq20 Ki;

    unsigned char Enabled;
    _iq20 Node0;
} PI_t;

typedef struct
{
    _iq20 PosSat;
    _iq20 NegSat;
    _iq20 Kp;
    _iq20 Ki;

    unsigned char Enabled;
    Phasor_t Node0;
} PIc_t;

_iq20 PI_Run(PI_t *pic, _iq20 val);
Phasor_t PI_RunPhasor(PIc_t *pic, Phasor_t val);

#endif
