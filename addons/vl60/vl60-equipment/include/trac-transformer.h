#ifndef     TRAC_TRANSFORMER_H
#define     TRAC_TRANSFORMER_H

#include    "device.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct position_t
{
    int     number;
    QString name;
    double  Unom;

    position_t()
        : number(0)
        , name("0")
        , Unom(0.0)
    {

    }
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class TracTransformer : public Device
{
public:

    TracTransformer(QObject *parent = Q_NULLPTR);

    ~TracTransformer();

    /// Получить напряжение с обмотки собственных нужд (СН)
    double getU_sn() const;

    /// Задать напряжение первичной обмотки
    void setU1(double value);

    /// Вернуть текущее напряжение на тяговой обмотке
    double getTracVoltage() const;

    QString getPosName() const;

    /// Задать текущую позицию ЭКГ
    void setPosition(int position);

private:


    /// Напряжение на первичной обмотке
    double  U1;    

    /// Коэффициент трансформации обмотки СН
    double  K_sn;

    /// Текущая позиция ЭКГ
    int     ekg_pos;

    /// Описатель текущей позиции ЭКГ
    position_t cur_pos;

    /// Данные о эквивалентных напряжениях тяговой обмотки на каждой позиции ЭКГ
    QMap<int, position_t>   position;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);
};

#endif // TRAC_TRANSFORMER_H
