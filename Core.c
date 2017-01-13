#include "Core.h"
#include "Interface.h"
#include "PI.h"
#include "Phasor.h"

// Parameters
// Max Uac for shortcut
#define SAFE_VOLT _IQ20(4)
// Reasonable impedance region
#define MIN_R _IQ20(0.5)
#define MAX_R _IQ20(300.0)
#define MIN_X _IQ20(2.0)
#define MAX_X _IQ20(5.0)
// Min Udc/Uac for current loop mode
#define MIN_UDC_UAC _IQ20(1.5)

// State
State_t g_State = S_IDLE;

// Target
_iq20 g_TargetDCvoltage = 0;
_iq20 g_TargetCosPhi = _IQ20(1);
Phasor_t g_TargetACcurrent = {0, 0};

// Measurement
_iq20 g_ACvoltageRms = 0;
Phasor_t g_ACvoltage = {0, 0};
static SinEst_t m_ACvoltage;

Phasor_t g_ACcurrent = {0, 0};
static SinEst_t m_ACcurrent;

Phasor_t g_Impedance = {0, 0};

_iq20 g_DCvoltage = 0;

// Current Loop Controller
static PIc_t m_PI = {
    _IQ20(+100),  // PosSat
    _IQ20(-100),  // NegSat
    _IQ20(0.3),   // Kp
    _IQ20(10e-4), // Ki
    0,            // Enabled
    {0, 0},       // Node0
};
static PIc_t m_PI2 = {
    _IQ20(+100),  // PosSat
    _IQ20(-100),  // NegSat
    _IQ20(1),     // Kp
    _IQ20(50e-4), // Ki
    0,            // Enabled
    {0, 0},       // Node0
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

                    // TODO
                    return 999;

                    g_State = S_FULL;
                    PIc_Enable(&m_PI);
                    PIc_Enable(&m_PI2);
                    return 0;
                default:
                    return -1;
            }
        case S_IMP:
            switch (st)
            {
                case S_IDLE:
                    g_State = S_IDLE;
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
                    // TODO
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
    g_DCvoltage = uDC; // TODO: Add filters

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
    temp1 = PIc_Run(&m_PI, curr);
    temp2 = PIc_Run(&m_PI2, temp1);

    // Unregulate
    temp3 = Pha_IRel(temp2, ref);

    // Target Voltage Phasor
    comp = Pha_Mul(temp2, g_Impedance);
    targ = Pha_Sub(g_ACvoltage, comp);

    // Target Voltage
    targV = _IQ20rmpy(_IQ20(1.414213562373095), targ.Im);

    // Ratio
    return _IQ20div(targV, g_DCvoltage);
}

void VoltageController()
{
    // TODO
}
