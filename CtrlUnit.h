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
#define ACOffset 16384 // Number in the parenthesis take Ampere as unit
#define DCGain _IQ30(0.015625)      // Number in the parenthesis take V/DCvoltage as unit
#define DCOffset _IQ22(0.0) // Number in the parenthesis take Voltage as unit
#define SampPeri _IQ30(0.0001) // Number in the parenthesis take second as unit)
//  PIcontroller definition
typedef struct
{
    _iq30 Numer[2];
    _iq30 Denom[2];
    _iq30 Gain;
} FilterCoeff;
typedef struct
{
    _iq20 Node1;
    _iq20 Node2;
} SecOrdFilter;
typedef struct
{
        _iq26  Err_Itgl;          // Variable: Error integration
        _iq26  ItglMax;           // Parameter: Maximum Integral
        _iq26  ItglMin;           // Parameter: Minimum Integral
        _iq15  Kp;                // Parameter: Proportional gain
        _iq15  Ki;                // Parameter: Integral gain
        _iq26  Out;               // Output: PID output
        _iq26  OutMax;            // Parameter: Maximum output
        _iq26  OutMin;            // Parameter: Minimum output
}  PICtrlr;

#define ACItglMax _IQ26(1)
#define ACItglMin _IQ26(-1)
#define ACOutMax _IQ26(0.9375)
#define ACOutMin _IQ26(-0.9375)

#define DCKp     _IQ28(2.5) // TODO
#define DCKi     _IQ29(0.5)
#define DCItglMax _IQ26(16)
#define DCItglMin _IQ26(-16)
#define DCOutMax _IQ26(16)
#define DCOutMin _IQ26(-16)

#define PhCom    _IQ30(3.0/360.0) //Phase Compensation

extern Uint64  g_SysCount;
void Processing(int16 newACcurrent, int16 newDCvoltage);

void SetPhaseZero(void);
void SetPhasePI(void);

void SetDanger(void);
void ClearDanger(void);
void OpenLoopCtrl(void);
void CloseLoopCtrl(void);

void AdjPhaseDelay(int dir);
void AdjSetVol(int dir);
void AdjACRef(int dir);
void AdjACKp(int dir);
void AdjACKi(int dir);
void Adjcmproffset(int dir);

extern long DataToSend;
extern unsigned char SendFlag;

#endif /* CTRLUNIT_H_ */
