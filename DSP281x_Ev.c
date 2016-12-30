#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

//---------------------------------------------------------------------------
// InitEv:
//---------------------------------------------------------------------------
// This function initializes to a known state.
//
void InitEv(void)
{
    // Frequency of General-purpose Timer1 is 150MHz
    EvaRegs.GPTCONA.bit.T1TOADC=1;  //Setting of underflow interrupt flag starts ADC
    EvaRegs.GPTCONA.bit.TCMPOE=1;   //Timer compare output: timer compare logic
    EvaRegs.GPTCONA.bit.T1PIN=2;    //Polarity of GP timer 1 compare output: Active high

    EvaRegs.T1CON.bit.FREE=0;
    EvaRegs.T1CON.bit.SOFT=1;   //Stop after current timer period is complete on emulation suspend
    EvaRegs.T1CON.bit.TMODE=1;  //Continuous-Up/-Down Count Mode
    EvaRegs.T1CON.bit.TPS=0;    //Input clock pre-scaler
    EvaRegs.T1CON.bit.TCLKS10=0;//Clock source:HSPCLK
    EvaRegs.T1CON.bit.TENABLE=1;//Timer enable
    EvaRegs.T1CON.bit.TCLD10=1; //Timer compare register reload condition
    // When counter value is 0 or equals period register value
    EvaRegs.T1CON.bit.TECMPR=1; //Enable timer compare operation

    EvaRegs.COMCONA.bit.CENABLE=1;  //Compare enable
    EvaRegs.COMCONA.bit.CLD=1;      //Compare register CMPRx reload condition:
    // When T1CNT = 0 or T1CNT = T1PR (that is, on underflow or period match)
    EvaRegs.COMCONA.bit.SVENABLE=0; //Disables space vector PWM mode
    EvaRegs.COMCONA.bit.ACTRLD=1;   //Action control register reload condition
    EvaRegs.COMCONA.bit.FCOMPOE=1;  //Full compare outputs, PWM1/2/3/4/5/6, are driven by corresponding compare logic

    EvaRegs.ACTRA.all=0x0666;   //0x0666 or 0x0999

    EvaRegs.DBTCONA.bit.DBT=15; //Dead-band timer period
    EvaRegs.DBTCONA.bit.EDBT1=1;
    EvaRegs.DBTCONA.bit.EDBT2=1;
    EvaRegs.DBTCONA.bit.DBTPS=7;//Dead-band timer prescaler

    EvaRegs.CMPR1=3000;
    EvaRegs.CMPR2=2000;
    // Width of Deadband is 6.4us.

    EvaRegs.T1PR = 7499;    // Timer1 period
    EvaRegs.T1CMPR=3749;    // D=50%
    EvaRegs.T1CNT=0x3749;

    EvaRegs.EVAIMRC.bit.CAP1INT=1;  //Enable CAP1 interrupt
    EvaRegs.EVAIMRC.bit.CAP2INT=1;  //Enable CAP2 interrupt

    EvaRegs.CAPCONA.bit.CAPRES=1;   // Capture Reset
    EvaRegs.CAPCONA.bit.CAP12EN=1;  //Enable captures 1 and 2
    EvaRegs.CAPCONA.bit.CAP3EN=0;   //Disable captures 3
    EvaRegs.CAPCONA.bit.CAP12TSEL=1;//Selects GP timer 1 for captures 1 and 2
    EvaRegs.CAPCONA.bit.CAP1EDGE=1; //Captures 1 detects rising edge
    EvaRegs.CAPCONA.bit.CAP2EDGE=2; //Captures 2 detects falling edge
    EvaRegs.CAPFIFOA.bit.CAP2FIFO=1;// CAP2FIFO has one entry
    //the EV module is led to "believe" that there is already an entry in the FIFO.
    //Subsequently, every time the FIFO gets a new value, a capture interrupt will be generated.
    EvaRegs.CAPFIFOA.bit.CAP1FIFO=1;
    EvaRegs.CAPFIFOA.bit.CAP1FIFO=1;

}
