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

const FilterCoeff AC1 =
{
    _IQ30(0),
    _IQ30(-1),
    _IQ30(-1.991208300207381),
    _IQ30(0.99125145496597722),
    _IQ30(0.057291205723457118)
};
const FilterCoeff AC2 =
{
    _IQ30(0),
    _IQ30(-1),
    _IQ30(-1.838801833563001),
    _IQ30(0.85210829515935227),
    _IQ30(0.057291205723457118)
};
SecOrdFilter AC1s={0,0},AC2s={0,0};
_iq20 SecOrdFil(const FilterCoeff* fc, SecOrdFilter* sof, _iq20 input);

void PI_reset(PICtrlr *v);
_iq26 PI_calc(PICtrlr *v, _iq20 Errset);

Uint64 g_SysCount = 0;
static _iq22 DCvoltage_log[3] = {0,0,0}; // -2^9 ~ 2^9-2^-22


static _iq22 SetVol = 12;                        // Defined in SCI ISR.
static _iq26 DCvoltageError_log[3] = {0,0,0};
static _iq26 DCvoltageError_1st[3] = {0,0,0}; // After Notch filter
static _iq26 DCvoltageError_2nd[2] = {0,0}; // After Low-pass filter


PICtrlr DCvoltagePICtrlr={0,DCItglMax,DCItglMin,DCKp,DCKi,0,DCOutMax,DCOutMin};

static _iq30 ACPhase       = 0x00000000;
static _iq30 SetPhaseDelay = 0x00000000;
static _iq30 DesiredPhase  = _IQ30(0);
static _iq20 ACcurrentRef  = 0x00000000;
static _iq26 ACcurrentError_log[3] = {0,0,0};

PICtrlr ACcurrentPICtrlr={0,ACItglMax,ACItglMin,ACKp,ACKi,0,ACOutMax,ACOutMin};

extern unsigned char Danger = 0x00;

void ControlledRect();
void UncontrolledRect();
void Processing(int16 newACcurrent, int16 newDCvoltage)
{
    int16 cmpr;
    _iq26 DCtemp;
    _iq20 ACtemp;
    _iq26 ACctrl;

    g_SysCount++;
    ACPhase += DeltaPhase;
    DesiredPhase = ACPhase + SetPhaseDelay;

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
    ACcurrentRef = _IQ20mpyIQX(_IQ26(1), 26, _IQ30sinPU(DesiredPhase), 30);
    // Generate current reference.
//    cmpr = _IQ15mpyIQX((ACcurrentRef+_IQ26(1.0)), 26, _IQ18(3750), 18)>>15;
//    EvaRegs.CMPR3 = cmpr;
//    // Display current reference through PWM5.

    ACtemp = _IQ20mpyIQX(ACGain, 30, (long)newACcurrent, 0) + ACOffset;
    ACtemp = SecOrdFil(&AC1, &AC1s, ACtemp);
    ACtemp = SecOrdFil(&AC2, &AC2s, ACtemp);
    cmpr = _IQ15mpyIQX((ACcurrentRef+_IQ20(1.0)), 20, _IQ18(3750), 18)>>15;
    EvaRegs.CMPR3 = cmpr;
    // Display current observation through PWM5.
    if ((ACtemp>_IQ20(0.7))||(ACtemp<_IQ20(-0.7)))
    {
       // Danger = 0xFF;
       // UncontrolledRect();
       // return;
    }

    ACtemp = ACcurrentRef-ACtemp;
    ACcurrentError_log[2]=ACcurrentError_log[1];
    ACcurrentError_log[1]=ACcurrentError_log[0];
    ACcurrentError_log[0]=ACtemp;

    ACctrl = PI_calc(&ACcurrentPICtrlr, ACtemp);

    if (Danger)
        return;

    cmpr = _IQ15mpyIQX((ACctrl+_IQ26(1.0)), 26, _IQ18(3750), 18)>>15;
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

void ControlledRect()
{
    EALLOW;
    GpioMuxRegs.GPAMUX.all  = 0x031F;
    GpioMuxRegs.GPADIR.all  = 0x006F;
    GpioDataRegs.GPASET.all = 0x0040; // Disable the current-limiting resistor
    EDIS;
}

void UncontrolledRect()
{
    EALLOW;
    GpioMuxRegs.GPAMUX.all    = 0x0310;
    GpioMuxRegs.GPADIR.all    = 0x006F;
    GpioDataRegs.GPACLEAR.all = 0x004F;
    EDIS;
}

void SetDanger(void)
{
    Danger=0xFF;
    UncontrolledRect();
    PI_reset(&ACcurrentPICtrlr);
}

void ClearDanger(void)
{
    Danger=0x00;
}

void PI_reset(PICtrlr *v)
{
    v->Err_Itgl = 0x00000000;
}

_iq26 PI_calc(PICtrlr *v, _iq20 Errset)
{
    v->Err_Itgl += _IQ26mpyIQX(Errset, 20, SampPeri, 30);
    if (v->Err_Itgl > v->ItglMax)
      v->Err_Itgl =  v->ItglMax;
    else if (v->Err_Itgl < v->ItglMin)
      v->Err_Itgl =  v->ItglMin;

    v->Out = _IQ26mpyIQX(v->Ki, 28, v->Err_Itgl, 26) + _IQ26mpyIQX(v->Kp, 28, Errset, 20);
    if (v->Out > v->OutMax)
      v->Out = v->OutMax;
    else if (v->Out < v->OutMin)
      v->Out = v->OutMin;
    return v->Out;
}

_iq20 SecOrdFil(const FilterCoeff* fc, SecOrdFilter* sof, _iq20 input)
{
    _iq20 temp1,temp2;
    temp1 = _IQ20mpyIQX(fc->Gain, 30, input, 20);
    temp1 -= _IQ20mpyIQX(fc->Denom[0],30,sof->Node1,20);
    temp1 -= _IQ20mpyIQX(fc->Denom[1],30,sof->Node2,20);
    temp2 = temp1;
    temp2 += _IQ20mpyIQX(fc->Numer[0],30,sof->Node1,20);
    temp2 += _IQ20mpyIQX(fc->Numer[1],30,sof->Node2,20);
    sof->Node2 = sof->Node1;
    sof->Node1 = temp1;
    return temp2;
}

void AdjPhaseDelay(int dir)
{
    if(dir>0)
        SetPhaseDelay += _IQ30(10.0/360.0);
    else if(dir<0)
        SetPhaseDelay -= _IQ30(10.0/360.0);
    else
        SetPhaseDelay = 0;
}

void AdjSetVol(int dir)
{
    if(dir>0)
        SetVol += _IQ22(5.0);
    else if(dir<0)
        SetVol -= _IQ22(5.0);
    else
        SetVol = 0;
}
