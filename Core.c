#include "Core.h"
#include "Interface.h"

// Max Uac for shortcut
#define SAFE_VOLT _IQ20(4)
// Reasonable impedance region
#define MIN_R _IQ20(0.5)
#define MAX_R _IQ20(300.0)
#define MIN_X _IQ20(2.0)
#define MAX_X _IQ20(5.0)

_iq20 g_ACvoltageRms = 0;
Phasor_t g_ACvoltage = {0, 0};
static SinEst_t m_ACvoltage;

Phasor_t g_ACcurrent = {0, 0};
static SinEst_t m_ACcurrent;

Phasor_t g_Impedance = {0, 0};

_iq20 g_DCvoltage = 0;

State_t g_State = S_IDLE;

int ChangeState(State_t st)
{
    switch (g_State)
    {
        case S_IDLE:
            switch (st)
            {
                case S_IDLE:
                    return 0;
                case S_IMP:
                    if (g_ACvoltageRms > SAFE_VOLT)
                        return 1;
                    g_State = S_IMP;
                    IF_SetPwm(0); // Shortcut AC
                    return 0;
                case S_RUN:
                    if (g_Impedance.Re < MIN_R ||
                        g_Impedance.Re > MAX_R ||
                        g_Impedance.Im < MIN_X ||
                        g_Impedance.Im > MAX_X)
                        return 1;
                    g_State = S_RUN;
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
                case S_RUN:
                    return 1;
                default:
                    return -1;
            }
        case S_RUN:
            switch (st)
            {
                case S_IDLE:
                    g_State = S_IDLE;
                    return 0;
                case S_IMP:
                    return 1;
                case S_RUN:
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
    g_ACvoltage = Sin_Run(&m_ACvoltage, uAC);
    g_ACvoltageRms = Pha_Rms(g_ACvoltage);
    g_ACcurrent = Sin_Run(&m_ACcurrent, iAC);
    g_DCvoltage = uDC; // TODO: Add filters
}
