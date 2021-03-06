#ifndef INTERFACE_H_
#define INTERFACE_H_
#include "IQmathLib.h"

// level == +1 means 100%
// level ==  0 means  50%
// level == -1 means   0%
void IF_SetPwm(_iq20 level);

// shutdown all IGBTs
void IF_Off();

void RawProcess(long uAC, long iAC, long uDC);

#endif
