/*
 * DigitalSignalProcessing.c
 *
 *  Created on: 2016-11-26
 *      Author: Noah
 *
 */
#include "DSP281x_Device.h"
#include "DSP281x_Examples.h"
#include "IQmathLib.h"
#include "DigitalSignalProcessing.h"

_iq26 ACcurrent[3]={0,0,0}; //-2^5 ~ 2^5-2^-26
_iq22 DCvoltage[3]={0,0,0}; //-2^9 ~ 2^9-2^-22

_iq22 SetVol;//Defined in SCI ISR.
_iq25 DCvoltageError_1st[3]={0,0,0};//Notch filter
_iq25 DCvoltageError_2nd[3]={0,0,0};//Low-pass filter, PI

_iq30 ACPhase=0x00000000;
_iq30 SetPhaseDelay=0x00000000;
_iq30 DesiredPhase=0x00000000;

_iq26 ACcurrentRef=0x00000000;
_iq29 ACcurrentError_1st[3]={0,0,0};


void Processing(int16 newACcurrent, int16 newDCvoltage)
{
    _iq25 DCtemp;
    _iq29 ACtemp;

    ACPhase += DeltaPhase;

    ACcurrent[2]=ACcurrent[1];
    ACcurrent[1]=ACcurrent[0];
    ACcurrent[0]=_IQ26mpyIQX(ACGain, 30, ((long)newACcurrent), 0)+ACOffset;

    DCvoltage[2]=DCvoltage[1];
    DCvoltage[1]=DCvoltage[0];
    DCvoltage[0]=_IQ22mpyIQX(DCGain, 30, ((long)newDCvoltage), 0)+DCOffset;

    if (DCvoltage[0]<_IQ22(160))
    {
        EALLOW;
        GpioMuxRegs.GPAMUX.all=0x0040;
        GpioMuxRegs.GPADIR.all=0x00FF;
        GpioDataRegs.GPACLEAR.all=0x00FF;
        EDIS;
    }
    else
    {
        EALLOW;
        GpioMuxRegs.GPAMUX.all=0x004F;
        GpioMuxRegs.GPADIR.all=0x0030;
        GpioDataRegs.GPASET.all=0x0010; //Disable the current-limiting resistor
        EDIS;

        DCtemp=-_IQ25mpyIQX(a2_1st, 27, DCvoltageError_1st[1], 25)-_IQ25mpyIQx(a3_1st, 27, DCvoltageError_1st[2], 25)
                +_IQ25mpyIQx(b1_1st, 27, (DCvoltage[0]-SetVol), 22)+_IQ25mpyIQx(b2_1st, 27, (DCvoltage[1]-SetVol), 22)+_IQ25mpyIQx(b3_1st, 27, (DCvoltage[2]-SetVol), 22);
        DCvoltageError_1st[2]=DCvoltageError_1st[1];
        DCvoltageError_1st[1]=DCvoltageError_1st[0];
        DCvoltageError_1st[0]=DCtemp;

        DCtemp=-_IQ25mpyIQx(a2_2nd, 27, DCvoltageError_2nd[1], 25)-_IQ25mpyIQx(a3_2nd, 27, DCvoltageError_2nd[2], 25)
                +_IQ25mpyIQx(b1_2nd, 27, DCvoltageError_1st[0], 25)+_IQ25mpyIQx(b2_2nd, 27, DCvoltageError_1st[1], 25)+_IQ25mpyIQx(b3_2nd, 27, DCvoltageError_1st[2], 25);
        DCvoltageError_2nd[2]=DCvoltageError_2nd[1];
        DCvoltageError_2nd[1]=DCvoltageError_2nd[0];
        DCvoltageError_2nd[0]=DCtemp;
        //DCvoltage_2nd and DCtemp is the ACcurrent amplitude reference

        DesiredPhase = ACPhase+SetPhaseDelay;
        ACcurrentRef=_IQ26mpyIQx(DCtemp, 25, _IQ30sin(DesiredPhase), 30);

        ACtemp=-_IQ29mpyIQx(a2_3rd, 27, ACcurrentError_1st[1], 29)-_IQ29mpyIQx(a3_3rd, 27, ACcurrentError_1st[2], 29)
                +_IQ29mpyIQx(b1_3rd, 27, (ACcurrent[0]-ACcurrentRef), 26)+_IQ29mpyIQx(b2_3rd, 27, (ACcurrent[1]-ACcurrentRef), 26)+_IQ29mpyIQx(b3_3rd, 27, (ACcurrent[2]-ACcurrentRef), 26);
        ACcurrentError_1st[2]=ACcurrentError_1st[1];
        ACcurrentError_1st[1]=ACcurrentError_1st[0];
        ACcurrentError_1st[0]=ACtemp;
        //Assumption: ACtemp varies from -1 to 1.

        EvaRegs.CMPR1=_IQ15mpyIQx((ACtemp+_IQ29(1.0)), 29, _IQ18(3750))>>15;
        EvaRegs.CMPR2=EvaRegs.CMPR1;
    }
}
void SetPhaseZero(void)
{
    ACPhase=0x00000000; //0 in _iq30
}
void SetPhasePI(void)
{
    ACPhase=0x40000000; //1 in _iq30
}
void SetVol_DPhi(Uint8 Vol, int8 DPhi)
{
    // Assumptions: DPhi:(-2^7 ~ 2^7-1)*2^2 //_iq-2
    SetVol=((long)Vol)<<24; // Turn to _iq22
    // Assumptions: DPhi:(-2^7 ~ 2^7-1)*2^-9//_iq9
    SetPhaseDelay=((long)DPhi)<<21; // Turn to _iq30
}
