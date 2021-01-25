#include    "phase-splitter.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
PhaseSplitter::PhaseSplitter(QObject *parent) : Device(parent)
  , Mmax(48.96)
  , s_kr(0.022)
  , Un(380.0)
  , omega0(157.08)
  , Mxx(1.5)
  , J(0.02)
  , U_power(0.0)
  , omega_r(141.4)
  , is_not_ready(1.0f)
  , k_eds(2.389)
  , U_out(0.0)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
PhaseSplitter::~PhaseSplitter()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void PhaseSplitter::setU_power(double value)
{
    if (floor(value) > 0 && floor(U_power) == 0)
        emit soundPlay("Phase_Splitter");

    if (floor(value) == 0 && floor(U_power) > 0)
        emit soundStop("Phase_Splitter");

    U_power = value;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float PhaseSplitter::isNotReady() const
{
    return is_not_ready;
}

double PhaseSplitter::getU_out() const
{
    return U_out;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void PhaseSplitter::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(Y)
    Q_UNUSED(t)

    is_not_ready = static_cast<float>(hs_n(Y[0] - omega_r));

    U_out = k_eds * Y[0];

    //emit soundSetPitch("Phase_Splitter", static_cast<float>(Y[0] / omega0));
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void PhaseSplitter::ode_system(const state_vector_t &Y,
                               state_vector_t &dYdt,
                               double t)
{
    Q_UNUSED(t)

    // Расчитывает текущее скольжение ротора
    double s = 1 - Y[0] / omega0;

    // Рачитываем максимальный момент при данном напряжении питания
    double M_maximal = Mmax * pow(U_power / Un, 2.0);

    // Расчитываем электромагнитный момент (формула Клосса)
    double Ma = 2 * M_maximal / ( s / s_kr + s_kr / s );

    // Расчитываем момент сил сопротивления
    double Mr = Physics::fricForce(Mxx, Y[0]);

    // Раcчитываем угловое ускорения вращающейся части
    dYdt[0] = (Ma - Mr) / J;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void PhaseSplitter::load_config(CfgReader &cfg)
{
    Q_UNUSED(cfg)
}
