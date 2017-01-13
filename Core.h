#ifndef CORE_H_
#define CORE_H_

#include "Phasor.h"

typedef enum
{
    Idle,
    ImpEst,
    Running
} State_t;

extern Phasor_t g_ACvoltage;
extern Phasor_t g_ACcurrent;
extern Phasor_t g_Impedance;

extern State_t g_State;

// Return: 0 is OK
int ChangeState(State_t st);

void Process(unsigned long uAC, unsigned long iAC, unsigned long uDC);

#endif
