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
#include "IQmathLib.h"

#define ACGain _IQ30(0.001953125)   // Number in the parenthesis take A/ACcurrent as units
#define ACOffset _IQ26(0.0) // Number in the parenthesis take Ampere as units
#define DCGain _IQ30(0.015625)      // Number in the parenthesis take V/DCvoltage as units
#define DCOffset _IQ22(0.0) // Number in the parenthesis take Voltage as units

#define a1_1st _IQ27(1.0)
#define a2_1st _IQ27(1.0)
#define a3_1st _IQ27(1.0)
#define b1_1st _IQ27(1.0)
#define b2_1st _IQ27(1.0)
#define b3_1st _IQ27(1.0)

#define a1_2nd _IQ27(1.0)
#define a2_2nd _IQ27(1.0)
#define a3_2nd _IQ27(1.0)
#define b1_2nd _IQ27(1.0)
#define b2_2nd _IQ27(1.0)
#define b3_2nd _IQ27(1.0)

#define a1_3rd _IQ27(1.0)
#define a2_3rd _IQ27(1.0)
#define a3_3rd _IQ27(1.0)
#define b1_3rd _IQ27(1.0)
#define b2_3rd _IQ27(1.0)
#define b3_3rd _IQ27(1.0)

_iq26 ACcurrent[3] = {0,0,0}; // -2^5 ~ 2^5-2^-26
_iq22 DCvoltage[3] = {0,0,0}; // -2^9 ~ 2^9-2^-22

_iq22 SetVol;                          // Defined in SCI ISR.
_iq25 DCvoltageError_1st[3] = {0,0,0}; // Notch filter
_iq25 DCvoltageError_2nd[3] = {0,0,0}; // Low-pass filter, PI

_iq30 ACPhase       = 0x00000000;
_iq30 SetPhaseDelay = 0x00000000;
_iq30 DesiredPhase  = 0x00000000;

_iq26 ACcurrentRef          = 0x00000000;
_iq29 ACcurrentError_1st[3] = {0,0,0};

void ControlledRect();
void UncontrolledRect();
int16 Filters(void);

void Processing(int16 newACcurrent, int16 newDCvoltage)
{
    int16 cmpr;

    ACPhase += DeltaPhase;

    ACcurrent[2] = ACcurrent[1];
    ACcurrent[1] = ACcurrent[0];
    ACcurrent[0] = _IQ26mpyIQX(ACGain, 30, (long)newACcurrent, 0) + ACOffset;

    DCvoltage[2] = DCvoltage[1];
    DCvoltage[1] = DCvoltage[0];
    DCvoltage[0] = _IQ22mpyIQX(DCGain, 30, (long)newDCvoltage, 0) + DCOffset;

    if (DCvoltage[0]<_IQ22(160))
    {
        UncontrolledRect();
        return;
    }

    cmpr = Filters();

    EvaRegs.CMPR1 = cmpr;
    EvaRegs.CMPR2 = cmpr;
    ControlledRect();
}

int16 Filters(void)
{
    _iq25 DCtemp;
    _iq29 ACtemp;

    DCtemp =
        -_IQ25mpyIQX(a2_1st, 27, DCvoltageError_1st[1], 25)
        -_IQ25mpyIQX(a3_1st, 27, DCvoltageError_1st[2], 25)
        +_IQ25mpyIQX(b1_1st, 27, (DCvoltage[0]-SetVol), 22)
        +_IQ25mpyIQX(b2_1st, 27, (DCvoltage[1]-SetVol), 22)
        +_IQ25mpyIQX(b3_1st, 27, (DCvoltage[2]-SetVol), 22);
    DCvoltageError_1st[2] = DCvoltageError_1st[1];
    DCvoltageError_1st[1] = DCvoltageError_1st[0];
    DCvoltageError_1st[0] = DCtemp;

    DCtemp =
        -_IQ25mpyIQX(a2_2nd, 27, DCvoltageError_2nd[1], 25)
        -_IQ25mpyIQX(a3_2nd, 27, DCvoltageError_2nd[2], 25)
        +_IQ25mpyIQX(b1_2nd, 27, DCvoltageError_1st[0], 25)
        +_IQ25mpyIQX(b2_2nd, 27, DCvoltageError_1st[1], 25)
        +_IQ25mpyIQX(b3_2nd, 27, DCvoltageError_1st[2], 25);
    DCvoltageError_2nd[2] = DCvoltageError_2nd[1];
    DCvoltageError_2nd[1] = DCvoltageError_2nd[0];
    DCvoltageError_2nd[0] = DCtemp;
    // DCvoltage_2nd and DCtemp is the ACcurrent amplitude reference

    DesiredPhase = ACPhase + SetPhaseDelay;
    ACcurrentRef = _IQ26mpyIQX(DCtemp, 25, _IQ30sin(DesiredPhase), 30);

    ACtemp =
        -_IQ29mpyIQX(a2_3rd, 27, ACcurrentError_1st[1]      , 29)
        -_IQ29mpyIQX(a3_3rd, 27, ACcurrentError_1st[2]      , 29)
        +_IQ29mpyIQX(b1_3rd, 27, (ACcurrent[0]-ACcurrentRef), 26)
        +_IQ29mpyIQX(b2_3rd, 27, (ACcurrent[1]-ACcurrentRef), 26)
        +_IQ29mpyIQX(b3_3rd, 27, (ACcurrent[2]-ACcurrentRef), 26);
    ACcurrentError_1st[2] = ACcurrentError_1st[1];
    ACcurrentError_1st[1] = ACcurrentError_1st[0];
    ACcurrentError_1st[0] = ACtemp;
    // Assumption: ACtemp varies from -1 to 1.

    return _IQ15mpyIQX((ACtemp+_IQ29(1.0)), 29, _IQ18(3750), 18)>>15;
}

void SetPhaseZero(void)
{
    ACPhase = 0x00000000; // 0 in _iq30
}

void SetPhasePI(void)
{
    ACPhase = 0x40000000; // 1 in _iq30
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
