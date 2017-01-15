#include "PI.h"

void PI_Enable(PI_t *pic)
{
    pic->Enabled = 1;
    pic->Node0   = 0;
}

void PIc_Enable(PIc_t *pic)
{
    pic->Enabled  = 1;
    pic->Node0.Re = 0;
    pic->Node0.Im = 0;
}

void PI_Disable(PI_t *pic)
{
    pic->Enabled = 0;
}

void PIc_Disable(PIc_t *pic)
{
    pic->Enabled = 0;
}

_iq20 PI_Run(PI_t *pic, _iq20 val)
{
    _iq20 temp = _IQ20rmpy(pic->Kp, val) + _IQ20rmpy(pic->Ki, pic->Node0);
    if (!pic->Enabled)
        pic->Node0 = 0;
    else
        pic->Node0 += val;
    if (pic->Node0 > pic->PosSat)
        pic->Node0 = pic->PosSat;
    else if (pic->Node0 < pic->NegSat)
        pic->Node0 = pic->NegSat;

    return temp;
}

Phasor_t PIc_Run(PIc_t *pic, Phasor_t val)
{
    Phasor_t temp = Pha_SMul(val, pic->Kp);
    temp = Pha_Add(temp, Pha_SMul(pic->Node0, pic->Ki));

    if (!pic->Enabled)
    {
        pic->Node0.Re = 0;
        pic->Node0.Im = 0;
    }
    else
        pic->Node0 = Pha_Add(pic->Node0, val);

    if (pic->Node0.Re > pic->PosSat)
        pic->Node0.Re = pic->PosSat;
    else if (pic->Node0.Re < pic->NegSat)
        pic->Node0.Re = pic->NegSat;
    if (pic->Node0.Im > pic->PosSat)
        pic->Node0.Im = pic->PosSat;
    else if (pic->Node0.Im < pic->NegSat)
        pic->Node0.Im = pic->NegSat;

    return temp;
}
