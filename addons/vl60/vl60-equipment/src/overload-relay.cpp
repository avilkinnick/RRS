#include    "overload-relay.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
OverloadRelay::OverloadRelay(QObject *parent) : Device(parent)
  , current(0.0)
  , trig_current(800.0)
  , state(0.0)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
OverloadRelay::~OverloadRelay()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void OverloadRelay::setCurrent(double value)
{
    current = value;
}

double OverloadRelay::getState() const
{
    return state;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void OverloadRelay::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)

    state = hs_p(qAbs(current) - trig_current);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void OverloadRelay::ode_system(const state_vector_t &Y,
                               state_vector_t &dYdt,
                               double t)
{
    Q_UNUSED(Y)
    Q_UNUSED(dYdt)
    Q_UNUSED(t)
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void OverloadRelay::load_config(CfgReader &cfg)
{
    cfg.getDouble("Device", "TrigCurren", trig_current);
}
