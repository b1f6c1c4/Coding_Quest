#ifndef CORE_H_
#define CORE_H_

#include "Phasor.h"
#include "SinEst.h"

typedef enum
{
    S_IDLE,
    S_IMP,
    S_RUN
} State_t;

extern State_t g_State;

_iq20 g_ACvoltageRms;
extern Phasor_t g_ACvoltage;
extern Phasor_t g_ACcurrent;
extern Phasor_t g_Impedance;
extern _iq20 g_DCvoltage;

// Return: 0 is OK
int ChangeState(State_t st);

void Process(_iq20 uAC, _iq20 iAC, _iq20 uDC);

#endif
