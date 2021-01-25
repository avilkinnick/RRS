#include "generator.h"
#include <QDir>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Generator::Generator(QObject* parent) : Device(parent)
  , Uf(0.0)
  , Lf(0.01)
  , Rf(0.0)

  , omega(0.0)
  , E(0.0)

  , La(0.01)
  , Ra(0.0)
  , Rt(0.0)

  , M(0.0)
  , Ut(0.0)

  , Rgp(0.0)
  , Rdp(0.0)
  , omega_nom(73.0)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Generator::~Generator()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Generator::preStep(state_vector_t& Y, double t)
{
    Q_UNUSED(t)

    M = calcCPhi((Y[0])) * Y[1];
    Ut = Y[1] * Rt;

    emit soundSetPitch("TED", static_cast<float>(abs(omega) / omega_nom));
    emit soundSetVolume("TED", static_cast<int>(pf(abs(Y[0]) - 100)));
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Generator::ode_system(const state_vector_t& Y, state_vector_t& dYdt, double t)
{
    Q_UNUSED(t)

    E = calcCPhi(Y[0]) * omega;

    dYdt[0] = (Uf - Y[0] * 6 * Rf) / Lf;

    dYdt[1] = -(E + Y[1] * (Ra + Rt)) / La;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Generator::load_config(CfgReader& cfg)
{
    QString secName = "Device";

    cfg.getDouble(secName, "R_a", Ra);
    cfg.getDouble(secName, "R_gp", Rgp);
    cfg.getDouble(secName, "R_dp", Rdp);
    cfg.getDouble(secName, "omega_nom", omega_nom);

    Rf = Rgp + Rdp;

    QString cPhiFileName = "";

    cfg.getString(secName, "cPhi", cPhiFileName);

    cPhi.load((custom_config_dir + QDir::separator() + cPhiFileName).toStdString());
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double Generator::calcCPhi(double I)
{
    return 1.2 * cPhi.getValue(I);
}
