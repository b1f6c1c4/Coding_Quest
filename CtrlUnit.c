/*
 * CtrlUnit.c
 *
 *  Created on: 2016-11-26
 *      Author: Noah
 *
 */
#include "DSP281x_Device.h"
#include "DSP281x_Examples.h"
#include "CtrlUnit.h"

Uint64 g_SysCount = 0;
static _iq26 ACcurrent_log[3] = {0,0,0}; // -2^5 ~ 2^5-2^-26
static _iq22 DCvoltage_log[3] = {0,0,0}; // -2^9 ~ 2^9-2^-22


static _iq22 SetVol;                        // Defined in SCI ISR.
static _iq26 DCvoltageError_log[3] = {0,0,0};
static _iq26 DCvoltageError_1st[3] = {0,0,0}; // After Notch filter
static _iq26 DCvoltageError_2nd[2] = {0,0}; // After Low-pass filter
PICtrlr DCvoltagePICtrlr={0,0,DCItglMax,DCItglMin,DCKp,DCKi,0,DCOutMax,DCOutMin};

static _iq30 ACPhase       = 0x00000000;
static _iq30 SetPhaseDelay = 0x00000000;
static _iq30 DesiredPhase  = 0x00000000;
static _iq26 ACcurrentRef  = 0x00000000;
static _iq26 ACcurrentError_log[3] = {0,0,0};
PICtrlr ACcurrentPICtrlr={0,0,ACItglMax,ACItglMin,ACKp,ACKi,0,ACOutMax,ACOutMin};

static unsigned char Danger = 0x00;

void ControlledRect();
void UncontrolledRect();
void Processing(int16 newACcurrent, int16 newDCvoltage)
{
    int16 cmpr;
    _iq26 DCtemp;
    _iq26 ACtemp;

    g_SysCount++;
    ACPhase += DeltaPhase;
    DesiredPhase = ACPhase + SetPhaseDelay;

//    if (Danger)
//        return;

    DCvoltage_log[2] = DCvoltage_log[1];
    DCvoltage_log[1] = DCvoltage_log[0];
    DCvoltage_log[0] = _IQ22mpyIQX(DCGain, 30, (long)newDCvoltage, 0) + DCOffset;

    // if (DCvoltage[0]<_IQ22(160))
    // {
       // UncontrolledRect();
       // return;
    // }

    DCtemp = SetVol-newDCvoltage;
    DCvoltageError_log[2]=DCvoltageError_log[1];
    DCvoltageError_log[1]=DCvoltageError_log[0];
    DCvoltageError_log[0]=DCtemp;

    // DCtemp =
       // -_IQ26mpyIQX(_IQ30(-1.519242529784681), 30, DCvoltageError_1st[1], 26)
       // -_IQ26mpyIQX(_IQ30(0.522244357021068), 30, DCvoltageError_1st[2], 26)
       // +_IQ26mpyIQX(_IQ30(0.761122178510534), 30, DCvoltageError_log[0], 26)
       // +_IQ26mpyIQX(_IQ30(-1.519242529784681), 30, DCvoltageError_log[1], 26)
       // +_IQ26mpyIQX(_IQ30(0.761122178510534), 30, DCvoltageError_log[2], 26);
    // DCvoltageError_1st[2] = DCvoltageError_1st[1];
    // DCvoltageError_1st[1] = DCvoltageError_1st[0];
    // DCvoltageError_1st[0] = DCtemp;

    // DCtemp =
       // -_IQ26mpyIQX(_IQ30(-0.990049751243781), 30, DCvoltageError_2nd[1], 26)
       // +_IQ26mpyIQX(_IQ30(0.004975124378109), 30, DCvoltageError_1st[0], 26)
       // +_IQ26mpyIQX(_IQ30(0.004975124378110), 30, DCvoltageError_1st[1], 26);
    // DCvoltageError_2nd[1] = DCvoltageError_2nd[0];
    // DCvoltageError_2nd[0] = DCtemp;
    // DCtemp = PI_calc(&DCvoltagePICtrlr, DCtemp);
    // DCtemp is the ACcurrent amplitude reference

    // ACcurrentRef = _IQ26mpyIQX(DCtemp, 26, _IQ30sinPU(DesiredPhase), 30);
    ACcurrentRef = _IQ26mpyIQX(_IQ26(0.8), 26, _IQ30sinPU(DesiredPhase), 30);

    ACcurrent_log[2] = ACcurrent_log[1];
    ACcurrent_log[1] = ACcurrent_log[0];
    ACcurrent_log[0] = _IQ26mpyIQX(ACGain, 30, (long)newACcurrent, 0) + ACOffset;

//    if ((ACcurrent_log[0]>_IQ26(0.7))||(ACcurrent_log[0]<_IQ26(-0.7)))
//    {
//        Danger = 0xFF;
//        UncontrolledRect();
//        return;
//    }

    ACtemp = ACcurrentRef-ACcurrent_log[0];
    ACcurrentError_log[2]=ACcurrentError_log[1];
    ACcurrentError_log[1]=ACcurrentError_log[0];
    ACcurrentError_log[0]=ACtemp;

    ACtemp = PI_calc(&ACcurrentPICtrlr, ACtemp);
//    cmpr = _IQ15mpyIQX((ACtemp+_IQ26(1.0)), 26, _IQ18(3750), 18)>>15;
    cmpr = _IQ15mpyIQX((ACcurrentRef+_IQ26(1.0)), 26, _IQ18(3750), 18)>>15;
    EvaRegs.CMPR1 = cmpr;
    EvaRegs.CMPR2 = cmpr;
    ControlledRect();
}

void SetPhaseZero(void)
{
    ACPhase = _IQ30(0)+PhCom; // 0 in _iq30
}

void SetPhasePI(void)
{
    ACPhase = _IQ30(0.5)+PhCom; // 0.5 in _iq30
}

void SetVol_DPhi(Uint8 Vol, int8 DPhi)
{
    // Assumptions: DPhi:(-2^7 ~ 2^7-1)*2^2  // _iq-2
    SetVol = ((long)Vol)<<24;         // Turn to _iq22
    // Assumptions: DPhi:(-2^7 ~ 2^7-1)*2^-9 // _iq9
    SetPhaseDelay = ((long)DPhi)<<21; // Turn to _iq30
}

void ControlledRect()
{
    EALLOW;
    GpioMuxRegs.GPAMUX.all  = 0x034F;
    GpioMuxRegs.GPADIR.all  = 0x003F;
    GpioDataRegs.GPASET.all = 0x0010; // Disable the current-limiting resistor
    EDIS;
}

void UncontrolledRect()
{
    EALLOW;
    GpioMuxRegs.GPAMUX.all    = 0x0340;
    GpioMuxRegs.GPADIR.all    = 0x003F;
    GpioDataRegs.GPACLEAR.all = 0x001F;
    EDIS;
}

void PI_reset(PICtrlr *v)
{
    v->Err_Itgl = 0x00000000;
}

_iq26 PI_calc(PICtrlr *v, _iq26 Errset)
{
    v->Err = Errset;
    v->Err_Itgl += _IQ26mpyIQX(Errset, 26, SampPeri, 30);
    if (v->Err_Itgl > v->ItglMax)
      v->Err_Itgl =  v->ItglMax;
    else if (v->Err_Itgl < v->ItglMin)
      v->Err_Itgl =  v->ItglMin;

    v->Out = _IQ26mpyIQX(v->Ki, 28, v->Err_Itgl, 26) + _IQ26mpyIQX(v->Kp, 28, v->Err, 26);
    if (v->Out > v->OutMax)
      v->Out = v->OutMax;
    else if (v->Out < v->OutMin)
      v->Out = v->OutMin;
    return v->Out;
}
