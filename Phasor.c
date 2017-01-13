#include "Phasor.h"

_iq20 Pha_Rms(Phasor_t p)
{
    return _IQ20mag(p.Re, p.Im);
}

_iq20 Pha_Phase(Phasor_t p)
{
    return _IQ20atan2PU(p.Re, p.Im);
}

Phasor_t Pha_Norm(Phasor_t p)
{
    _iq20 mag = Pha_Rms(p);
    p.Re = _IQ20div(p.Re, mag);
    p.Im = _IQ20div(p.Im, mag);
    return p;
}

Phasor_t Pha_Add(Phasor_t p1, Phasor_t p2)
{
    Phasor_t p;
    p.Re = p1.Re + p2.Re;
    p.Im = p1.Im + p2.Im;
    return p;
}

Phasor_t Pha_Sub(Phasor_t p1, Phasor_t p2)
{
    Phasor_t p;
    p.Re = p1.Re - p2.Re;
    p.Im = p1.Im - p2.Im;
    return p;
}

Phasor_t Pha_Mul(Phasor_t p1, Phasor_t p2)
{
    Phasor_t p;
    p.Re = _IQ20rmpy(p1.Re, p2.Re) - _IQ20rmpy(p1.Im, p2.Im);
    p.Im = _IQ20rmpy(p1.Im, p2.Re) + _IQ20rmpy(p1.Re, p2.Im);
    return p;
}

Phasor_t Pha_Div(Phasor_t p1, Phasor_t p2)
{
    _iq20 mag2;
    Phasor_t p;

    mag2 = _IQ20rmpy(p2.Re, p2.Re) + _IQ20rmpy(p2.Im, p2.Im);
    pBase.Re = _IQ20div(p2.Re, mag);
    pBase.Im = _IQ20div(p2.Im, mag);

    p.Re = _IQ20rmpy(p1.Re, p2.Re) + _IQ20rmpy(p1.Im, p2.Im);
    p.Im = _IQ20rmpy(p1.Im, p2.Re) - _IQ20rmpy(p1.Re, p2.Im);
    return p;
}

Phasor_t Pha_Rel(Phasor_t p1, Phasor_t pBase)
{
    Phasor_t p;
    p.Re = _IQ20rmpy(p1.Re, pBase.Re) + _IQ20rmpy(p1.Im, pBase.Im);
    p.Im = _IQ20rmpy(p1.Im, pBase.Re) - _IQ20rmpy(p1.Re, pBase.Im);
    return p;
}

Phasor_t Pha_IRel(Phasor_t p1, Phasor_t pBase)
{
    Phasor_t p;
    p.Re = _IQ20rmpy(p1.Re, pBase.Re) - _IQ20rmpy(p1.Im, pBase.Im);
    p.Im = _IQ20rmpy(p1.Im, pBase.Re) + _IQ20rmpy(p1.Re, pBase.Im);
    return p;
}
