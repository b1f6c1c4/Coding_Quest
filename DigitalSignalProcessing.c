/*
 * DigitalSignalProcessing.c
 *
 *  Created on: 2016-11-26
 *      Author: Noah
 */
#define GLOBAL_Q 19
#include "DSP281x_Device.h"
#include "DSP281x_Examples.h"
#include "IQmathLib.h"
#include "DigitalSignalProcessing.h"

#define PI _IQ(3.141592653)
#define ADCGND _IQ(1.5000)
#define Ref_1  _IQ(1.0000)
#define Ref_2  _IQ(2.0000)
#define DeltaRef    _IQ(1.0000)

int16 newACvoltage;
int16 newACcurrent;
int16 newDCvoltage;
int16 newRef_1;
int16 newRef_2;
_iq ACvoltage[3]={0,0,0};
_iq ACcurrent[3]={0,0,0};

//_iq Gain=0;
//_iq Offset=0;
#define Gain _IQ(1.0)
#define Offset _IQ(1.0)

_iq temp=0;
_iq DCvoltage[3]={0,0,0};
extern _iq SettingVoltage;//Defined in SCI ISR.
_iq DCvoltageError[3]={0,0,0};
_iq DCvoltageError_1st[3]={0,0,0};//Notch filter
_iq DCvoltageError_2nd[3]={0,0,0};//Low-pass filter, PI

extern _iq ACPhase=0;
extern _iq DesiredPhaseDelay;// Defined in SCI ISR.
_iq DesiredPhase=0;

_iq ACcurrentRef=0x0000;
_iq ACcurrentError[3]={0,0,0};
_iq ACcurrentError_1st[3]={0,0,0};

long temp2;

void Processing(void)
{
	ACPhase +=_IQmpy(_IQ(0.0005),PI);// 10us / 20ms
//    Gain=_IQdiv(DeltaRef,_Q1toIQ(newRef_2-newRef_1));
//    Offset=_IQdiv(_IQmpy(_Q1toIQ(newRef_2),Ref_1)-_IQmpy(_Q1toIQ(newRef_1),Ref_2),_Q1toIQ(newRef_2-newRef_1));

    ACvoltage[2]=ACvoltage[1];
    ACvoltage[1]=ACvoltage[0];
    temp=_IQmpy(Gain,newACvoltage)+Offset-ADCGND;//-1.5-1.5V
    ACvoltage[0]=_IQmpy(temp,_IQ(100))-150;//
    if ((ACvoltage[2]<0) && (ACvoltage[0]>0))
    	ACPhase=0;
    else if((ACvoltage[2]>0) && (ACvoltage[0]<0))
    	ACPhase=PI;

    DCvoltage[2]=DCvoltage[1];
    DCvoltage[1]=DCvoltage[0];
    temp=_IQmpy(Gain,newDCvoltage)+Offset-ADCGND;//0-3V
    DCvoltage[0]=_IQmpy(temp,_IQ(100))-150;

    if (DCvoltage[0]>160)
    {
    	EALLOW;
    	GpioMuxRegs.GPAMUX.all=0x004F;
    	GpioMuxRegs.GPADIR.all=0x0010;
    	GpioDataRegs.GPASET.all=0x0010;
    	EDIS;
    	ACcurrent[2]=ACcurrent[1];
		ACcurrent[1]=ACcurrent[0];
		temp=_IQmpy(Gain,newACcurrent)+Offset-ADCGND;//0-3V
		ACcurrent[0]=_IQmpy(temp,_IQ(5))-2.5;

		temp=-_IQmpy(a2_1st,DCvoltageError_1st[1])-_IQmpy(a3_1st,DCvoltageError_1st[2])+_IQmpy(b1_1st,DCvoltage[0]-SettingVoltage)+_IQmpy(b2_1st,DCvoltage[1]-SettingVoltage)+_IQmpy(b3_1st,DCvoltage[2]-SettingVoltage);
		DCvoltageError_1st[2]=DCvoltageError_1st[1];
		DCvoltageError_1st[1]=DCvoltageError_1st[0];
		DCvoltageError_1st[0]=temp;

		temp=-_IQmpy(a2_2nd,DCvoltageError_2nd[1])-_IQmpy(a3_2nd,DCvoltageError_2nd[2])+_IQmpy(b1_2nd,DCvoltageError_1st[0])+_IQmpy(b2_2nd,DCvoltageError_1st[1])+_IQmpy(b3_2nd,DCvoltageError_1st[2]);
		DCvoltageError_2nd[2]=DCvoltageError_2nd[1];
		DCvoltageError_2nd[1]=DCvoltageError_2nd[0];
		DCvoltageError_2nd[0]=temp;
		//DCvoltage_2nd and temp is the ACcurrent amplitude reference

		DesiredPhase = ACPhase+DesiredPhaseDelay;
		ACcurrentRef=_IQmpy(temp,_IQsin(DesiredPhase));

		temp=-_IQmpy(a2_3rd,ACcurrentError_1st[1])-_IQmpy(a3_3rd,ACcurrentError_1st[2])+_IQmpy(b1_3rd,ACcurrent[0]-ACcurrentRef)+_IQmpy(b2_3rd,ACcurrent[1]-ACcurrentRef)+_IQmpy(b3_3rd,ACcurrent[2]-ACcurrentRef);
		ACcurrentError_1st[2]=ACcurrentError_1st[1];
		ACcurrentError_1st[1]=ACcurrentError_1st[0];
		ACcurrentError_1st[0]=temp;

		EvaRegs.T1CMPR=(unsigned int)_IQint(_IQrsmpy(ACcurrentError_1st[0]+_IQ(1),_IQ(3750)));

    }
    else
    {
        EALLOW;
        GpioMuxRegs.GPAMUX.all=0x0040;
        GpioMuxRegs.GPADIR.all=0x001F;
        GpioDataRegs.GPACLEAR.all=0x001F;
        GpioMuxRegs.GPFMUX.bit.SCIRXDA_GPIOF5=1;
        EDIS;
    }
    //temp is the drive logic of switch
}
