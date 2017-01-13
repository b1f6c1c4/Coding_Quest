#ifndef PHASOR_H_
#define PHASOR_H_

#include "FixedPoint.h"

typedef struct
{
    _iq20 Re;
    _iq20 Im;
} Phasor_t;

_iq20 Pha_Rms(Phasor_t p);
_iq20 Pha_Phase(Phasor_t p);
Phasor_t Pha_SMul(Phasor_t p1, _iq20 val);
Phasor_t Pha_SDiv(Phasor_t p1, _iq20 val);
Phasor_t Pha_Norm(Phasor_t p);

Phasor_t Pha_Add(Phasor_t p1, Phasor_t p2);
Phasor_t Pha_Sub(Phasor_t p1, Phasor_t p2);
Phasor_t Pha_Mul(Phasor_t p1, Phasor_t p2);
Phasor_t Pha_Div(Phasor_t p1, Phasor_t p2);
Phasor_t Pha_Rel(Phasor_t p1, Phasor_t pBase);
Phasor_t Pha_IRel(Phasor_t p1, Phasor_t pBase);

#endif
