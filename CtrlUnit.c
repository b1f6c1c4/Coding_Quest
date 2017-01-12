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
    _IQ30(-1.9915249934537753),
    _IQ30(0.99157152241187263      ),
    _IQ30(0.027722042757883023      )
};
const FilterCoeff AC2 =
{
    _IQ30(0),
    _IQ30(-1),
    _IQ30(-1.9277693004408991       ),
    _IQ30(0.93098913281474049      ),
    _IQ30(0.027722042757883023      )
};
static SecOrdFilter AC1s={0,0},AC2s={0,0};
_iq20 SecOrdFil(const FilterCoeff* fc, SecOrdFilter* sof, _iq20 input);

const _iq30 ACfir[] =
{
        _IQ30(0.5),
        _IQ30(0.25),
        _IQ30(0.125),
        _IQ30(0.125)
};
static _iq20 ACfir_d[] = {0,0,0,0};
_iq20 FirFil(const _iq30* fc, _iq20* sof, int n, _iq20 input);

void PI_reset(PICtrlr *v);
_iq26 PI_calc(PICtrlr *v, _iq20 Errset);

Uint64 g_SysCount = 0;
static _iq22 DCvoltage_log[3] = {0,0,0}; // -2^9 ~ 2^9-2^-22

static _iq22 SetVol = 12;                        // Defined in SCI ISR.

PICtrlr DCvoltagePICtrlr={0,DCItglMax,DCItglMin,DCKp,DCKi,0,DCOutMax,DCOutMin};

static _iq30 ACPhase       = 0x00000000;
static _iq30 SetPhaseDelay = 0x00000000;
static _iq30 DesiredPhase  = _IQ30(0);
static _iq20 ACcurrentRef  = 0x00000000;
static _iq26 ACRef_ampli = _IQ26(0);
static int cmproffset = 0;

#define ACKp _IQ15(0)
#define ACKi _IQ15(0)



PICtrlr ACcurrentPICtrlr={0,ACItglMax,ACItglMin,ACKp,ACKi,0,ACOutMax,ACOutMin};

extern unsigned char Danger = 0xFF;

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
    ACcurrentRef = _IQ20mpyIQX(ACRef_ampli, 26, _IQ30sinPU(DesiredPhase), 30);
    // Generate current reference.
//    cmpr = _IQ15mpyIQX((ACcurrentRef+_IQ26(1.0)), 26, _IQ18(3750), 18)>>15;
//    EvaRegs.CMPR3 = cmpr;
//    // Display current reference through PWM5.

    ACtemp = _IQ20mpyIQX(ACGain, 30, (long)newACcurrent - ACOffset, 0) ;
    ACtemp = SecOrdFil(&AC1, &AC1s, ACtemp);
    ACtemp = SecOrdFil(&AC2, &AC2s, ACtemp);
    //ACtemp = FirFil(ACfir, ACfir_d, sizeof(ACfir_d)/sizeof(*ACfir_d), ACtemp);

    // Display current observation through PWM5.
//    if ((ACtemp>_IQ20(0.7))||(ACtemp<_IQ20(-0.7)))
//    {
//        Danger = 0xFF;
//        UncontrolledRect();
//        return;
//    }

    ACtemp = ACcurrentRef-ACtemp;
    cmpr = _IQ15mpyIQX((ACcurrentRef+_IQ20(1.0)), 20, _IQ18(3750), 18)>>15;
    EvaRegs.CMPR3 = cmpr;

    ACctrl = PI_calc(&ACcurrentPICtrlr, ACtemp);

    if (Danger)
        return;

//    cmpr = _IQ15mpyIQX((ACctrl+_IQ26(1.0)), 26, _IQ18(3750), 18)>>15;
    EvaRegs.CMPR1 = cmpr + cmproffset;
    EvaRegs.CMPR2 = cmpr + cmproffset;
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

    v->Out = _IQ26mpyIQX(v->Ki, 15, v->Err_Itgl, 26) + _IQ26mpyIQX(v->Kp, 15, Errset, 20);
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

_iq20 FirFil(const _iq30* fc, _iq20* sof, int n, _iq20 input)
{
    _iq20 temp = 0;
    int i;
    for (i = n - 1; i > 0; i--)
    {
        sof[i] = sof[i - 1];
        temp += _IQ20mpyIQX(fc[i],30, sof[i],20);
    }
    sof[0] = input;
    temp += _IQ20mpyIQX(fc[0],30, sof[0],20);
    return temp;
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

void AdjACRef(int dir)
{
    if(dir>0)
        ACRef_ampli += _IQ26(0.05);
    else if(dir<0)
        ACRef_ampli -= _IQ26(0.05);
    else
        ACRef_ampli = 0;
}

void AdjACKp(int dir)
{
    if(dir>0)
        ACcurrentPICtrlr.Kp += _IQ15(1);
    else if(dir<0)
        ACcurrentPICtrlr.Kp -= _IQ15(1);
    else
        ACcurrentPICtrlr.Kp = ACKp;
}

void AdjACKi(int dir)
{
    if(dir>0)
        ACcurrentPICtrlr.Ki += _IQ15(250);
    else if(dir<0)
        ACcurrentPICtrlr.Ki -= _IQ15(250);
    else
        ACcurrentPICtrlr.Ki = ACKi;
}


void Adjcmproffset(int dir)
{
    if(dir>0)
        cmproffset += 5;
    else if(dir<0)
        cmproffset -= 5;
    else
        cmproffset = 0;
}
