#include "Core.h"
#include "Interface.h"
#include "PI.h"
#include "Phasor.h"
#include "FilterIIR.h"

// Parameters
// Max Uac for shortcut
// Unit: Volt
#define SAFE_VOLT _IQ20(4)
// Reasonable impedance region
// Unit: Ohm
#define MIN_R _IQ20(1)
#define MAX_R _IQ20(300.0)
#define MIN_X _IQ20(-20)
#define MAX_X _IQ20(-3)
// Min Udc/Uac for current loop mode
// Unit: 1
#define MIN_UDC_UAC _IQ20(1.5)
// Reasonable Udc target region
// Unit: 1
#define MAX_TARG_UDC_UAC _IQ20(2.8)
#define MIN_TARG_UDC_UAC _IQ20(1.0)
// Max target Iac
// Unit: Ampere
#define MAX_TARG_IAC _IQ20(5.0)

// State
State_t g_State = S_IDLE;

// Target
_iq20 g_TargetDCvoltage = 0;
_iq20 g_TargetSinPhi = 0;
Phasor_t g_TargetACcurrent = {0, 0};

// Measurement
_iq20 g_ACvoltageRms = 0;
Phasor_t g_ACvoltage = {0, 0};
static SinEst_t m_ACvoltage;

_iq20 g_ACcurrentRms = 0;
Phasor_t g_ACcurrent = {0, 0};
static SinEst_t m_ACcurrent;

Phasor_t g_Impedance = {0, 0};

_iq20 g_DCvoltage = 0;

// Current Loop Controller
#define MAX_PWM_RATIO _IQ20(0.96875)
static PIc_t m_PI = {
    _IQ20(+1000), // PosSat
    _IQ20(-1000), // NegSat
    _IQ20(0.6),   // Kp
    _IQ20(12e-5), // Ki
    0,            // Enabled
    {0, 0},       // Node0
};
static PIc_t m_PI2 = {
    _IQ20(+1000), // PosSat
    _IQ20(-1000), // NegSat
    _IQ20(0.6),   // Kp
    _IQ20(10e-3), // Ki
    0,            // Enabled
    {0, 0},       // Node0
};

// Voltage Loop Controller
#define VOLT_SCALE _IQ20(1e-4)
static PI_t m_PI3 = {
    _IQ20(+1000), // PosSat
    _IQ20(-1000), // NegSat
    _IQ20(100),   // Kp
    _IQ20(60e-3), // Ki
    0,            // Enabled
    0,            // Node0
};

// Public Functions

int CheckImpedance();
_iq20 CurrentController();
void VoltageController();

int ChangeState(State_t st)
{
    int ret;

    switch (g_State)
    {
        case S_IDLE:
            switch (st)
            {
                case S_IDLE:
                    return 0;
                case S_IMP:
                    if (g_ACvoltageRms > SAFE_VOLT)
                        return 2;
                    g_State = S_IMP;
                    IF_SetPwm(0); // Shortcut AC
                    return 0;
                case S_CURR:
                    ret = CheckImpedance();
                    if (ret)
                        return ret;
                    if (_IQ20rmpy(MIN_UDC_UAC, g_ACvoltageRms) > g_DCvoltage)
                        return 2;

                    g_State = S_CURR;
                    PIc_Enable(&m_PI);
                    PIc_Enable(&m_PI2);
                    return 0;
                case S_FULL:
                    ret = CheckImpedance();
                    if (ret)
                        return ret;

                    g_State = S_FULL;
                    PIc_Enable(&m_PI);
                    PIc_Enable(&m_PI2);
                    PI_Enable(&m_PI3);
                    return 0;
                default:
                    return -1;
            }
        case S_IMP:
            switch (st)
            {
                case S_IDLE:
                    g_State = S_IDLE;
                    IF_Off();
                    return 0;
                case S_IMP:
                    return 0;
                case S_CURR:
                    return 1;
                case S_FULL:
                    return 1;
                default:
                    return -1;
            }
        case S_CURR:
            switch (st)
            {
                case S_IDLE:
                    g_State = S_IDLE;
                    PIc_Disable(&m_PI);
                    PIc_Disable(&m_PI2);
                    IF_Off();
                    return 0;
                case S_IMP:
                    return 1;
                case S_CURR:
                    return 0;
                case S_FULL:
                    return 1;
                default:
                    return -1;
            }
        case S_FULL:
            switch (st)
            {
                case S_IDLE:
                    g_State = S_IDLE;
                    PIc_Disable(&m_PI);
                    PIc_Disable(&m_PI2);
                    PI_Disable(&m_PI3);
                    IF_Off();
                    return 0;
                case S_IMP:
                    return 1;
                case S_CURR:
                    return 1;
                case S_FULL:
                    return 0;
                default:
                    return -1;
            }
        default:
            return -1;
    }
}

void Process(_iq20 uAC, _iq20 iAC, _iq20 uDC)
{
    _iq20 ratio;

    g_ACvoltage = Sin_Run(&m_ACvoltage, uAC);
    g_ACvoltageRms = Pha_Rms(g_ACvoltage);
    g_ACcurrent = Sin_Run(&m_ACcurrent, iAC);
    g_ACcurrentRms = Pha_Rms(g_ACcurrent);
    g_DCvoltage = uDC;

    if (g_State == S_IDLE)
        return;

    if (g_State == S_IMP)
    {
        g_Impedance = Pha_Div(g_ACvoltage, g_ACcurrent);
        return;
    }

    if (g_State == S_FULL)
    {
        // g_TargetDCvoltage, g_TargetCosPhi will
        // be used to adjust g_TargetACcurrent
        VoltageController();
    }

    // g_TargetACcurrent will be used
    ratio = CurrentController();
    if (ratio > MAX_PWM_RATIO)
        ratio = MAX_PWM_RATIO;
    else if (ratio < -MAX_PWM_RATIO)
        ratio = -MAX_PWM_RATIO;
    IF_SetPwm(ratio);
}

// Private Functions

int CheckImpedance()
{
    if (g_Impedance.Re < MIN_R ||
        g_Impedance.Re > MAX_R ||
        g_Impedance.Im < MIN_X ||
        g_Impedance.Im > MAX_X)
        return 2;
    return 0;
}

_iq20 CurrentController()
{
    Phasor_t ref, curr;
    Phasor_t temp1, temp2, temp3;
    Phasor_t comp, targ;
    _iq20 targV;

    // Phase Reference
    ref = Pha_Norm(g_ACvoltage);

    // Regulate
    curr = Pha_Rel(g_ACcurrent, ref);

    // PI
    curr = Pha_Sub(curr, g_TargetACcurrent);
    temp1 = PIc_Run(&m_PI, curr);
    temp2 = PIc_Run(&m_PI2, temp1);

    // Unregulate
    temp3 = Pha_IRel(temp2, ref);

    // Target Voltage Phasor
    comp = Pha_Mul(temp3, g_Impedance);
    targ = Pha_Sub(g_ACvoltage, comp);

    // Target Voltage
    targV = _IQ20rmpy(_IQ20(1.414213562373095), targ.Im);

    // Ratio
    return _IQ20div(targV, g_DCvoltage);
}

void VoltageController()
{
    _iq20 targ, maxTarg, minTarg, err;

    // Bound
    targ = g_TargetDCvoltage;
    maxTarg = _IQ20rmpy(g_ACvoltageRms, MAX_TARG_UDC_UAC);
    minTarg = _IQ20rmpy(g_ACvoltageRms, MIN_TARG_UDC_UAC);
    if (targ > maxTarg)
        targ = maxTarg;
    else if (targ < minTarg)
        targ = minTarg;

    // Error
    err = targ - g_DCvoltage;

    // Scaled PI
    g_TargetACcurrent.Re = PI_Run(&m_PI3, _IQ20rmpy(err, VOLT_SCALE));

    // Saturation
    if (g_TargetACcurrent.Re > MAX_TARG_IAC)
        g_TargetACcurrent.Re = MAX_TARG_IAC;
    else if (g_TargetACcurrent.Re < -MAX_TARG_IAC)
        g_TargetACcurrent.Re = -MAX_TARG_IAC;

    // SinPhi
    g_TargetACcurrent.Im = _IQ20div(g_TargetACcurrent.Re, g_TargetSinPhi);
}
