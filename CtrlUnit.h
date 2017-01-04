/*
 * CtrlUnit.h
 *
 *  Created on: 2016-11-26
 *      Author: Noah
 */
#include "IQmathLib.h"
#ifndef CTRLUNIT_H_
#define CTRLUNIT_H_
//  PIcontroller definition
typedef struct
{
        _iq26  Err;               // Variable: Error
        _iq26  Err_Itgl;          // Variable: Error integration
        _iq26  ItglMax;           // Parameter: Maximum Integral
        _iq26  ItglMin;           // Parameter: Minimum Integral
        _iq28  Kp;                // Parameter: Proportional gain
        _iq28  Ki;                // Parameter: Integral gain
        _iq26  Out;               // Output: PID output
        _iq26  OutMax;            // Parameter: Maximum output
        _iq26  OutMin;            // Parameter: Minimum output
}  PICtrlr;
void PI_reset(PICtrlr *v);
_iq26 PI_calc(PICtrlr *v, _iq26 Errset);
#define DeltaPhase 536871   // 10us / 20ms * 2^30
void Processing(int16 newACcurrent, int16 newDCvoltage);
void SetPhaseZero(void);
void SetPhasePI(void);

#endif /* CTRLUNIT_H_ */
