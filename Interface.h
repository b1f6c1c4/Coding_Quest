#ifndef INTERFACE_H_
#define INTERFACE_H_
#include "IQmathLib.h"

// level == +1 means 100%
// level ==  0 means  50%
// level == -1 means   0%
void IF_SetPwm(_iq20 level);

// shutdown all IGBTs
void IF_Off();

extern unsigned long g_shit;
extern unsigned long g_fuck;
extern unsigned long g_bitch;

void RawProcess(long uAC, long iAC, long uDC);

#endif
