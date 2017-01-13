#include "Core.h"
#include "Interface.h"
#include "PI.h"
#include "Phasor.h"
#include "FilterIIR.h"

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
_iq20 g_TargetSinPhi = 0;
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
static PI_t m_PI3 = {
    _IQ20(+100),  // PosSat
    _IQ20(-100),  // NegSat
    _IQ20(1e-5),  // Kp
    _IQ20(10e-6), // Ki
    0,            // Enabled
    0,            // Node0
};

// DCvoltage IIR Filter
static IIR_t m_IIR[5] = {
    {
        // Numer
        {
            _IQ30(-1.9959545554580642),
            _IQ30(1)
        },
        // Denom
        {
            _IQ30(-1.9988294637104755),
            _IQ30( 0.99884318920216619)
        },
        // Gain
        _IQ30(0.003392826560444677),
        0, // Node1
        0  // Node2
    },
    {
        // Numer
        {
            _IQ30(-1.995030143398858),
            _IQ30(1)
        },
        // Denom
        {
            _IQ30(-1.9966306594202408),
            _IQ30( 0.9966443784761313)
        },
        // Gain
        _IQ30(0.0027604530656781781),
        0, // Node1
        0  // Node2
    },
    {
        // Numer
        {
            _IQ30(-1.992114693638525),
            _IQ30(1)
        },
        // Denom
        {
            _IQ30(-1.9947593314507899),
            _IQ30( 0.99477305167546459)
        },
        // Gain
        _IQ30(0.0017399735718241316),
        0, // Node1
        0  // Node2
    },
    {
        // Numer
        {
            _IQ30(-1.9809244979649847),
            _IQ30(1)
        },
        // Denom
        {
            _IQ30(-1.993397689134718),
            _IQ30( 0.99341141403984989)
        },
        // Gain
        _IQ30(0.00071950426817661648),
        0, // Node1
        0  // Node2
    },
    {
        // Numer
        {
            _IQ30(-1.8448334120218466),
            _IQ30(1)
        },
        // Denom
        {
            _IQ30(-1.9926807688086423),
            _IQ30( 0.99269449746983485)
        },
        // Gain
        _IQ30(0.00008847691614119181),
        0, // Node1
        0  // Node2
    }
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
                    PI_Disable(&m_PI3);
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
    g_DCvoltage = IIR_RunN(m_IIR, 5, uDC);

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
    _iq20 err;

    // Error
    err = g_TargetDCvoltage - g_DCvoltage;

    // PI
    g_TargetACcurrent.Re = PI_Run(&m_PI3, err);

    // SinPhi
    g_TargetACcurrent.Im = _IQ20div(g_TargetACcurrent.Re, g_TargetSinPhi);
}
