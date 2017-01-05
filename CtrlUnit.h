/*
 * CtrlUnit.h
 *
 *  Created on: 2016-11-26
 *      Author: Noah
 */
#include "IQmathLib.h"
#ifndef CTRLUNIT_H_
#define CTRLUNIT_H_
#define DeltaPhase 5368709   // 100us / 20ms * 2^30
#define ACGain _IQ30(0.0002275)   // Number in the parenthesis take A/ACcurrent as unit
#define ACOffset _IQ26(-3.4613) // Number in the parenthesis take Ampere as unit
#define DCGain _IQ30(0.015625)      // Number in the parenthesis take V/DCvoltage as unit
#define DCOffset _IQ22(0.0) // Number in the parenthesis take Voltage as unit
#define SampPeri _IQ30(0.0001) // Number in the parenthesis take second as unit)
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

#define ACKp     _IQ28(2.5)
#define ACKi     _IQ29(0.5)
#define ACItglMax _IQ26(16)
#define ACItglMin _IQ26(-16)
#define ACOutMax _IQ26(1)
#define ACOutMin _IQ26(-1)
#define DCKp     _IQ28(2.5)
#define DCKi     _IQ29(0.5)
#define DCItglMax _IQ26(16)
#define DCItglMin _IQ26(-16)
#define DCOutMax _IQ26(16)
#define DCOutMin _IQ26(-16)

void PI_reset(PICtrlr *v);
_iq26 PI_calc(PICtrlr *v, _iq26 Errset);

void Processing(int16 newACcurrent, int16 newDCvoltage);

void SetPhaseZero(void);
void SetPhasePI(void);

#endif /* CTRLUNIT_H_ */
