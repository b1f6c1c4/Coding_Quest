#ifndef CORE_H_
#define CORE_H_

#include "Phasor.h"
#include "SinEst.h"

typedef enum
{
    S_IDLE,
    S_IMP,
    S_CURR,
    S_FULL
} State_t;

// State
extern State_t g_State;

// Target
extern _iq20 g_TargetDCvoltage;
#define TARG_MIN_TANPHI _IQ20(-0.619744338403102)
#define TARG_MAX_TANPHI _IQ20(+0.619744338403102)
extern _iq20 g_TargetTanPhi;
extern Phasor_t g_TargetACcurrent; // DON'T MODIFY THIS IN CURR MODE

// Measurement
extern _iq20 g_ACvoltageRms;
extern Phasor_t g_ACvoltage;
extern _iq20 g_ACcurrentRms;
extern Phasor_t g_ACcurrent;
extern Phasor_t g_Impedance;
extern _iq20 g_DCvoltage;
extern unsigned long DataToSend;

// Return
// 0 : OK
// 1 : Go S_Idle First
// 2 : For Safety Consideration
int ChangeState(State_t st);

void Process(_iq20 uAC, _iq20 iAC, _iq20 uDC);

#endif
