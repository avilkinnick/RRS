#include    "auxiliary-converter.h"


//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
AuxiliaryConverter::AuxiliaryConverter(QObject *parent) : Device (parent)
  ,U4(0)
  ,Fref(1)
  ,Uref(1)
  ,Koef(0.1267)
  ,U1(0)
  ,F(0)
  ,U2(0)
{

}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
AuxiliaryConverter::~AuxiliaryConverter()
{

}

//------------------------------------------------------------------------------
// Установить значение от ПСН
//------------------------------------------------------------------------------
void AuxiliaryConverter::setU4(double U4)
{
        this->U4 = U4;
}

//------------------------------------------------------------------------------
// Получить значение напряжения
//------------------------------------------------------------------------------
double AuxiliaryConverter::getU1()
{
    return  U1;
}

//------------------------------------------------------------------------------
// Получить значение частоты
//------------------------------------------------------------------------------
double AuxiliaryConverter::getF()
{
    return  F;
}

//------------------------------------------------------------------------------
// Получить значение напряжения 380
//------------------------------------------------------------------------------
double AuxiliaryConverter::getU2()
{
    return U2;
}

//------------------------------------------------------------------------------
// Пердварительные шаги
//------------------------------------------------------------------------------
void AuxiliaryConverter::preStep(state_vector_t &Y, double t)
{
    U1 = Uref;

    F = Fref;

    U2 = Koef * U4;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void AuxiliaryConverter::ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t)
{

}

//------------------------------------------------------------------------------
// Загрузка конфига
//------------------------------------------------------------------------------
void AuxiliaryConverter::load_config(CfgReader &cfg)
{

}
