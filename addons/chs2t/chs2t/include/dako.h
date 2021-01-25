#ifndef DACO_H
#define DACO_H

#include "device.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class Dako : public Device
{
public:

    ///Конструктор
    Dako(QObject *parent = Q_NULLPTR);

    ///Деструктор
    ~Dako();

    void setPgr(double value)   { pgr = value; }
    void setQvr(double value)   { Qvr = value; }
    void setPtc(double value)   { ptc = value; }
    void setVelocity(double value)     { U = value; }
    void setPkvt(double value)  { p_kvt = value; }

    double getP1() const    { return getY(1); }
    double getPy() const    { return getY(0); }
    double getQtc() const   { return Qtc; }

    bool isEDTAllow() const { return  EDT_state; }

private:

    double V1;
    double Vy;
    double U1;
    double U2;

    double pgr;
    double Qvr;
    double ptc;
    double U;

    double py;
    double Qtc;
    double p_kvt;

    double A1;
    double A2;
    double A3;

    double K1;
    double K2;
    double K3;
    double K4;
    double K5;

    double k_1;
    double k_2;
    double k_3;
    double k_4;

    bool EDT_state;

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void preStep(state_vector_t &Y, double t);

    /// Загрузка данных из конфигурационных файлов
    void load_config(CfgReader &cfg);
};


#endif // DACO_H
