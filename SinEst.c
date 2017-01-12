#include "SinEst.h"

static _iq31 SinEst_Re[SINEST_N] = {
    _IQ30(-0.000222107607464),
    _IQ30(-0.000443996021534),
    _IQ30(-0.000665446265135),
    _IQ30(-0.000886239793614),
    _IQ30(-0.001106158710412),
    _IQ30(-0.001324985982112),
    _IQ30(-0.001542505652615),
    _IQ30(-0.001758503056268),
    _IQ30(-0.001972765029711),
    _IQ30(-0.002185080122244),
    _IQ30(-0.002395238804505),
    _IQ30(-0.002603033675246),
    _IQ30(-0.002808259666018),
    _IQ30(-0.003010714243545),
    _IQ30(-0.003210197609601),
    _IQ30(-0.003406512898189),
    _IQ30(-0.003599466369819),
    _IQ30(-0.003788867602711),
    _IQ30(-0.003974529680715),
    _IQ30(-0.004156269377775),
    _IQ30(-0.004333907338751),
    _IQ30(-0.004507268256423),
    _IQ30(-0.004676181044495),
    _IQ30(-0.004840479006438),
    _IQ30(-0.005000000000000),
    _IQ30(-0.005154586597219),
    _IQ30(-0.005304086239788),
    _IQ30(-0.005448351389608),
    _IQ30(-0.005587239674394),
    _IQ30(-0.005720614028177),
    _IQ30(-0.005848342826572),
    _IQ30(-0.005970300016676),
    _IQ30(-0.006086365241467),
    _IQ30(-0.006196423958581),
    _IQ30(-0.006300367553351),
    _IQ30(-0.006398093445998),
    _IQ30(-0.006489505192865),
    _IQ30(-0.006574512581594),
    _IQ30(-0.006653031720155),
    _IQ30(-0.006724985119640),
    _IQ30(-0.006790301770728),
    _IQ30(-0.006848917213772),
    _IQ30(-0.006900773602405),
    _IQ30(-0.006945819760633),
    _IQ30(-0.006984011233337),
    _IQ30(-0.007015310330143),
    _IQ30(-0.007039686162622),
    _IQ30(-0.007057114674771),
    _IQ30(-0.007067578666751),
    _IQ30(-0.007071067811865),
    _IQ30(-0.007067578666751),
    _IQ30(-0.007057114674771),
    _IQ30(-0.007039686162622),
    _IQ30(-0.007015310330143),
    _IQ30(-0.006984011233337),
    _IQ30(-0.006945819760633),
    _IQ30(-0.006900773602405),
    _IQ30(-0.006848917213772),
    _IQ30(-0.006790301770728),
    _IQ30(-0.006724985119640),
    _IQ30(-0.006653031720155),
    _IQ30(-0.006574512581594),
    _IQ30(-0.006489505192865),
    _IQ30(-0.006398093445998),
    _IQ30(-0.006300367553351),
    _IQ30(-0.006196423958581),
    _IQ30(-0.006086365241467),
    _IQ30(-0.005970300016676),
    _IQ30(-0.005848342826572),
    _IQ30(-0.005720614028177),
    _IQ30(-0.005587239674394),
    _IQ30(-0.005448351389608),
    _IQ30(-0.005304086239788),
    _IQ30(-0.005154586597219),
    _IQ30(-0.005000000000000),
    _IQ30(-0.004840479006438),
    _IQ30(-0.004676181044495),
    _IQ30(-0.004507268256423),
    _IQ30(-0.004333907338751),
    _IQ30(-0.004156269377775),
    _IQ30(-0.003974529680715),
    _IQ30(-0.003788867602711),
    _IQ30(-0.003599466369819),
    _IQ30(-0.003406512898189),
    _IQ30(-0.003210197609601),
    _IQ30(-0.003010714243545),
    _IQ30(-0.002808259666018),
    _IQ30(-0.002603033675246),
    _IQ30(-0.002395238804505),
    _IQ30(-0.002185080122244),
    _IQ30(-0.001972765029711),
    _IQ30(-0.001758503056268),
    _IQ30(-0.001542505652615),
    _IQ30(-0.001324985982112),
    _IQ30(-0.001106158710412),
    _IQ30(-0.000886239793614),
    _IQ30(-0.000665446265135),
    _IQ30(-0.000443996021534),
    _IQ30(-0.000222107607464),
    _IQ30(-0.000000000000000),
    _IQ30(0.000222107607464),
    _IQ30(0.000443996021534),
    _IQ30(0.000665446265135),
    _IQ30(0.000886239793614),
    _IQ30(0.001106158710412),
    _IQ30(0.001324985982112),
    _IQ30(0.001542505652615),
    _IQ30(0.001758503056268),
    _IQ30(0.001972765029711),
    _IQ30(0.002185080122244),
    _IQ30(0.002395238804505),
    _IQ30(0.002603033675246),
    _IQ30(0.002808259666018),
    _IQ30(0.003010714243545),
    _IQ30(0.003210197609601),
    _IQ30(0.003406512898189),
    _IQ30(0.003599466369819),
    _IQ30(0.003788867602711),
    _IQ30(0.003974529680715),
    _IQ30(0.004156269377775),
    _IQ30(0.004333907338751),
    _IQ30(0.004507268256423),
    _IQ30(0.004676181044495),
    _IQ30(0.004840479006438),
    _IQ30(0.005000000000000),
    _IQ30(0.005154586597219),
    _IQ30(0.005304086239788),
    _IQ30(0.005448351389608),
    _IQ30(0.005587239674394),
    _IQ30(0.005720614028177),
    _IQ30(0.005848342826572),
    _IQ30(0.005970300016676),
    _IQ30(0.006086365241467),
    _IQ30(0.006196423958581),
    _IQ30(0.006300367553351),
    _IQ30(0.006398093445998),
    _IQ30(0.006489505192865),
    _IQ30(0.006574512581594),
    _IQ30(0.006653031720155),
    _IQ30(0.006724985119640),
    _IQ30(0.006790301770728),
    _IQ30(0.006848917213772),
    _IQ30(0.006900773602405),
    _IQ30(0.006945819760633),
    _IQ30(0.006984011233337),
    _IQ30(0.007015310330143),
    _IQ30(0.007039686162622),
    _IQ30(0.007057114674771),
    _IQ30(0.007067578666751),
    _IQ30(0.007071067811865),
    _IQ30(0.007067578666751),
    _IQ30(0.007057114674771),
    _IQ30(0.007039686162622),
    _IQ30(0.007015310330143),
    _IQ30(0.006984011233337),
    _IQ30(0.006945819760633),
    _IQ30(0.006900773602405),
    _IQ30(0.006848917213772),
    _IQ30(0.006790301770728),
    _IQ30(0.006724985119640),
    _IQ30(0.006653031720155),
    _IQ30(0.006574512581594),
    _IQ30(0.006489505192865),
    _IQ30(0.006398093445998),
    _IQ30(0.006300367553351),
    _IQ30(0.006196423958581),
    _IQ30(0.006086365241467),
    _IQ30(0.005970300016676),
    _IQ30(0.005848342826572),
    _IQ30(0.005720614028177),
    _IQ30(0.005587239674394),
    _IQ30(0.005448351389608),
    _IQ30(0.005304086239788),
    _IQ30(0.005154586597219),
    _IQ30(0.005000000000000),
    _IQ30(0.004840479006438),
    _IQ30(0.004676181044495),
    _IQ30(0.004507268256423),
    _IQ30(0.004333907338751),
    _IQ30(0.004156269377775),
    _IQ30(0.003974529680715),
    _IQ30(0.003788867602711),
    _IQ30(0.003599466369819),
    _IQ30(0.003406512898189),
    _IQ30(0.003210197609601),
    _IQ30(0.003010714243545),
    _IQ30(0.002808259666018),
    _IQ30(0.002603033675246),
    _IQ30(0.002395238804505),
    _IQ30(0.002185080122244),
    _IQ30(0.001972765029711),
    _IQ30(0.001758503056268),
    _IQ30(0.001542505652615),
    _IQ30(0.001324985982112),
    _IQ30(0.001106158710412),
    _IQ30(0.000886239793614),
    _IQ30(0.000665446265135),
    _IQ30(0.000443996021534),
    _IQ30(0.000222107607464),
    _IQ30(0.000000000000000)
};

static SinEst_Im[SINEST_N] = {
    _IQ30(0.007067578666751),
    _IQ30(0.007057114674771),
    _IQ30(0.007039686162622),
    _IQ30(0.007015310330143),
    _IQ30(0.006984011233337),
    _IQ30(0.006945819760633),
    _IQ30(0.006900773602405),
    _IQ30(0.006848917213772),
    _IQ30(0.006790301770728),
    _IQ30(0.006724985119640),
    _IQ30(0.006653031720155),
    _IQ30(0.006574512581594),
    _IQ30(0.006489505192865),
    _IQ30(0.006398093445998),
    _IQ30(0.006300367553351),
    _IQ30(0.006196423958581),
    _IQ30(0.006086365241467),
    _IQ30(0.005970300016676),
    _IQ30(0.005848342826572),
    _IQ30(0.005720614028177),
    _IQ30(0.005587239674394),
    _IQ30(0.005448351389608),
    _IQ30(0.005304086239788),
    _IQ30(0.005154586597219),
    _IQ30(0.005000000000000),
    _IQ30(0.004840479006438),
    _IQ30(0.004676181044495),
    _IQ30(0.004507268256423),
    _IQ30(0.004333907338751),
    _IQ30(0.004156269377775),
    _IQ30(0.003974529680715),
    _IQ30(0.003788867602711),
    _IQ30(0.003599466369819),
    _IQ30(0.003406512898189),
    _IQ30(0.003210197609601),
    _IQ30(0.003010714243545),
    _IQ30(0.002808259666018),
    _IQ30(0.002603033675246),
    _IQ30(0.002395238804505),
    _IQ30(0.002185080122244),
    _IQ30(0.001972765029711),
    _IQ30(0.001758503056268),
    _IQ30(0.001542505652615),
    _IQ30(0.001324985982112),
    _IQ30(0.001106158710412),
    _IQ30(0.000886239793614),
    _IQ30(0.000665446265135),
    _IQ30(0.000443996021534),
    _IQ30(0.000222107607464),
    _IQ30(-0.000000000000000),
    _IQ30(-0.000222107607464),
    _IQ30(-0.000443996021534),
    _IQ30(-0.000665446265135),
    _IQ30(-0.000886239793614),
    _IQ30(-0.001106158710412),
    _IQ30(-0.001324985982112),
    _IQ30(-0.001542505652615),
    _IQ30(-0.001758503056268),
    _IQ30(-0.001972765029711),
    _IQ30(-0.002185080122244),
    _IQ30(-0.002395238804505),
    _IQ30(-0.002603033675246),
    _IQ30(-0.002808259666018),
    _IQ30(-0.003010714243545),
    _IQ30(-0.003210197609601),
    _IQ30(-0.003406512898189),
    _IQ30(-0.003599466369819),
    _IQ30(-0.003788867602711),
    _IQ30(-0.003974529680715),
    _IQ30(-0.004156269377775),
    _IQ30(-0.004333907338751),
    _IQ30(-0.004507268256423),
    _IQ30(-0.004676181044495),
    _IQ30(-0.004840479006438),
    _IQ30(-0.005000000000000),
    _IQ30(-0.005154586597219),
    _IQ30(-0.005304086239788),
    _IQ30(-0.005448351389608),
    _IQ30(-0.005587239674394),
    _IQ30(-0.005720614028177),
    _IQ30(-0.005848342826572),
    _IQ30(-0.005970300016676),
    _IQ30(-0.006086365241467),
    _IQ30(-0.006196423958581),
    _IQ30(-0.006300367553351),
    _IQ30(-0.006398093445998),
    _IQ30(-0.006489505192865),
    _IQ30(-0.006574512581594),
    _IQ30(-0.006653031720155),
    _IQ30(-0.006724985119640),
    _IQ30(-0.006790301770728),
    _IQ30(-0.006848917213772),
    _IQ30(-0.006900773602405),
    _IQ30(-0.006945819760633),
    _IQ30(-0.006984011233337),
    _IQ30(-0.007015310330143),
    _IQ30(-0.007039686162622),
    _IQ30(-0.007057114674771),
    _IQ30(-0.007067578666751),
    _IQ30(-0.007071067811865),
    _IQ30(-0.007067578666751),
    _IQ30(-0.007057114674771),
    _IQ30(-0.007039686162622),
    _IQ30(-0.007015310330143),
    _IQ30(-0.006984011233337),
    _IQ30(-0.006945819760633),
    _IQ30(-0.006900773602405),
    _IQ30(-0.006848917213772),
    _IQ30(-0.006790301770728),
    _IQ30(-0.006724985119640),
    _IQ30(-0.006653031720155),
    _IQ30(-0.006574512581594),
    _IQ30(-0.006489505192865),
    _IQ30(-0.006398093445998),
    _IQ30(-0.006300367553351),
    _IQ30(-0.006196423958581),
    _IQ30(-0.006086365241467),
    _IQ30(-0.005970300016676),
    _IQ30(-0.005848342826572),
    _IQ30(-0.005720614028177),
    _IQ30(-0.005587239674394),
    _IQ30(-0.005448351389608),
    _IQ30(-0.005304086239788),
    _IQ30(-0.005154586597219),
    _IQ30(-0.005000000000000),
    _IQ30(-0.004840479006438),
    _IQ30(-0.004676181044495),
    _IQ30(-0.004507268256423),
    _IQ30(-0.004333907338751),
    _IQ30(-0.004156269377775),
    _IQ30(-0.003974529680715),
    _IQ30(-0.003788867602711),
    _IQ30(-0.003599466369819),
    _IQ30(-0.003406512898189),
    _IQ30(-0.003210197609601),
    _IQ30(-0.003010714243545),
    _IQ30(-0.002808259666018),
    _IQ30(-0.002603033675246),
    _IQ30(-0.002395238804505),
    _IQ30(-0.002185080122244),
    _IQ30(-0.001972765029711),
    _IQ30(-0.001758503056268),
    _IQ30(-0.001542505652615),
    _IQ30(-0.001324985982112),
    _IQ30(-0.001106158710412),
    _IQ30(-0.000886239793614),
    _IQ30(-0.000665446265135),
    _IQ30(-0.000443996021534),
    _IQ30(-0.000222107607464),
    _IQ30(-0.000000000000000),
    _IQ30(0.000222107607464),
    _IQ30(0.000443996021534),
    _IQ30(0.000665446265135),
    _IQ30(0.000886239793614),
    _IQ30(0.001106158710412),
    _IQ30(0.001324985982112),
    _IQ30(0.001542505652615),
    _IQ30(0.001758503056268),
    _IQ30(0.001972765029711),
    _IQ30(0.002185080122244),
    _IQ30(0.002395238804505),
    _IQ30(0.002603033675246),
    _IQ30(0.002808259666018),
    _IQ30(0.003010714243545),
    _IQ30(0.003210197609601),
    _IQ30(0.003406512898189),
    _IQ30(0.003599466369819),
    _IQ30(0.003788867602711),
    _IQ30(0.003974529680715),
    _IQ30(0.004156269377775),
    _IQ30(0.004333907338751),
    _IQ30(0.004507268256423),
    _IQ30(0.004676181044495),
    _IQ30(0.004840479006438),
    _IQ30(0.005000000000000),
    _IQ30(0.005154586597219),
    _IQ30(0.005304086239788),
    _IQ30(0.005448351389608),
    _IQ30(0.005587239674394),
    _IQ30(0.005720614028177),
    _IQ30(0.005848342826572),
    _IQ30(0.005970300016676),
    _IQ30(0.006086365241467),
    _IQ30(0.006196423958581),
    _IQ30(0.006300367553351),
    _IQ30(0.006398093445998),
    _IQ30(0.006489505192865),
    _IQ30(0.006574512581594),
    _IQ30(0.006653031720155),
    _IQ30(0.006724985119640),
    _IQ30(0.006790301770728),
    _IQ30(0.006848917213772),
    _IQ30(0.006900773602405),
    _IQ30(0.006945819760633),
    _IQ30(0.006984011233337),
    _IQ30(0.007015310330143),
    _IQ30(0.007039686162622),
    _IQ30(0.007057114674771),
    _IQ30(0.007067578666751),
    _IQ30(0.007071067811865)
};

SinEst g_ACvoltage = {0};
SinEst g_ACcurrent = {0};

void RunSinEst(SinEst *est, _iq20 data)
{
    int i;
    _iq20 re = 0;
    _iq20 im = 0;
    for (i = 0; i < SINEST_N - 1; i++)
    {
        est->Node[i] = est->Node[i + 1];
        re += _IQ20mpyIQX(SinEst_Re[i], 30, est->Node[i], 20);
        im += _IQ20mpyIQX(SinEst_Im[i], 30, est->Node[i], 20);
    }
    est->Node[i] = data;
    re += _IQ20mpyIQX(SinEst_Re[i], 30, est->Node[i], 20);
    im += _IQ20mpyIQX(SinEst_Im[i], 30, est->Node[i], 20);

    est->Rms = _IQ20mag(re, im);
    est->Phase = _IQ20atan2PU(re, im);
}
