/*
 * DigitalSignalProcessing.c
 *
 *  Created on: 2016-11-26
 *      Author: Noah
 *
 */
#define GLOBAL_Q 19
#include "DSP281x_Device.h"
#include "DSP281x_Examples.h"
#include "IQmathLib.h"
#include "DigitalSignalProcessing.h"

_iq ACcurrent[3]={0,0,0};
_iq DCvoltage[3]={0,0,0};

_iq SetVol;//Defined in SCI ISR.
_iq DCvoltageError[3]={0,0,0};
_iq DCvoltageError_1st[3]={0,0,0};//Notch filter
_iq DCvoltageError_2nd[3]={0,0,0};//Low-pass filter, PI

_iq ACPhase=0x00000000;
_iq SetPhaseDelay=0x00000000;// Defined in SCI ISR.
_iq DesiredPhase;

_iq ACcurrentRef;
_iq ACcurrentError[3]={0,0,0};
_iq ACcurrentError_1st[3]={0,0,0};


void Processing(int16 newACcurrent, int16 newDCvoltage)
{
    _iq temp;

    ACPhase += DeltaPhase;

    ACcurrent[2]=ACcurrent[1];
    ACcurrent[1]=ACcurrent[0];
    ACcurrent[0]=_IQmpy(ACGain,(newACcurrent<<2))+ACOffset;

    DCvoltage[2]=DCvoltage[1];
    DCvoltage[1]=DCvoltage[0];
    DCvoltage[0]=_IQmpy(DCGain,newDCvoltage)+DCOffset;

    if (DCvoltage[0]<160)
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

        temp=-_IQmpy(a2_1st,DCvoltageError_1st[1])-_IQmpy(a3_1st,DCvoltageError_1st[2])+_IQmpy(b1_1st,DCvoltage[0]-SetVol)+_IQmpy(b2_1st,DCvoltage[1]-SetVol)+_IQmpy(b3_1st,DCvoltage[2]-SetVol);
        DCvoltageError_1st[2]=DCvoltageError_1st[1];
        DCvoltageError_1st[1]=DCvoltageError_1st[0];
        DCvoltageError_1st[0]=temp;

        temp=-_IQmpy(a2_2nd,DCvoltageError_2nd[1])-_IQmpy(a3_2nd,DCvoltageError_2nd[2])+_IQmpy(b1_2nd,DCvoltageError_1st[0])+_IQmpy(b2_2nd,DCvoltageError_1st[1])+_IQmpy(b3_2nd,DCvoltageError_1st[2]);
        DCvoltageError_2nd[2]=DCvoltageError_2nd[1];
        DCvoltageError_2nd[1]=DCvoltageError_2nd[0];
        DCvoltageError_2nd[0]=temp;
        //DCvoltage_2nd and temp is the ACcurrent amplitude reference

        DesiredPhase = ACPhase+SetPhaseDelay;
        ACcurrentRef=_IQmpy(temp,_IQsin(DesiredPhase));

        temp=-_IQmpy(a2_3rd,ACcurrentError_1st[1])-_IQmpy(a3_3rd,ACcurrentError_1st[2])+_IQmpy(b1_3rd,ACcurrent[0]-ACcurrentRef)+_IQmpy(b2_3rd,ACcurrent[1]-ACcurrentRef)+_IQmpy(b3_3rd,ACcurrent[2]-ACcurrentRef);
        ACcurrentError_1st[2]=ACcurrentError_1st[1];
        ACcurrentError_1st[1]=ACcurrentError_1st[0];
        ACcurrentError_1st[0]=temp;

        //EvaRegs.=(unsigned int)_IQint(_IQrsmpy(ACcurrentError_1st[0]+_IQ(1.0),_IQ(3750)));
        /Edited @ 19:
    }
}
void SetPhaseZero(void)
{
    ACPhase=0;
}
void SetPhasePI(void)
{
    ACPhase=_IQ(1);
}
void SetVol_DPhi(Uint8 Vol, int8 DPhi)
{
}
