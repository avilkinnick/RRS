#ifndef GENERATOR_H
#define GENERATOR_H

#include "device.h"
#include "motor-magnetic-char.h"

class Generator : public Device
{

public:

    ///Конструктор
    Generator(QObject *parent = Q_NULLPTR);

    ///Деструктор
    ~Generator();

    void setUf(double value) { Uf = value; }

    void setLf(double value) { Lf = value; }

    void setRf(double value) { Rf = value; }

    double getIf() const { return getY(0); }

    void setOmega(double value) { omega = value; }

    void setE(double value) { E = value; }

    double getIa() const { return  getY(1); }

    void setLa(double value) { La = value; }

    void setRa(double value) { Ra = value; }

    void setRt(double value) { Rt = value; }

    double getTorque() const { return M; }

    double getUt() const { return  Ut; }

private:

    double Uf;
    double Lf;
    double Rf;

    double omega;
    double E;

    double La;
    double Ra;
    double Rt;

    double M;
    double Ut;

    double Rgp;
    double Rdp;
    double omega_nom;

    MotorMagneticChar cPhi;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);

    double calcCPhi(double I);
};


#endif // GENERATOR_H
