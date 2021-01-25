#ifndef     MOTOR_COMPRESSOR_H
#define     MOTOR_COMPRESSOR_H

#include    "device.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class MotorCompressor : public Device
{
public:

    MotorCompressor(QString config_path, QObject *parent = Q_NULLPTR);

    ~MotorCompressor();

    void setExternalPressure(double press);

    double getAirFlow() const;

    void setU_power(double value);

private:

    /// Внешнее противодавление
    double  p;

    /// Расход возхуа
    double  Q;

    /// "Подача" компрессора
    double  p0;

    double  Mmax;

    double  s_kr;

    double  Un;

    double  U_power;

    double  omega0;

    double  J;

    double  Mxx;

    double  Vnk;

    enum
    {
        NUM_COEFFS = 6
    };

    std::array<double, NUM_COEFFS>  K;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);

    void load_config(QString cfg_path);
};

#endif // MOTOR_COMPRESSOR_H
