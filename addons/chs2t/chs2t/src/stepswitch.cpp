#include "stepswitch.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
StepSwitch::StepSwitch(QObject* parent) : Device(parent)
  , V(4.12)
  , V1(10.6)
  , poz_d(0)
  , poz(0)
  , n(0)
  , p(0)

{

}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
StepSwitch::~StepSwitch()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double StepSwitch::getSchemeState() const
{
    return static_cast<double>(poz != 0);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void StepSwitch::ode_system(const state_vector_t& Y, state_vector_t& dYdt, double t)
{

}

//------------------------------------------------------------------------------
// Загрузка данных из конфигурационного файла
//------------------------------------------------------------------------------
void StepSwitch::load_config(CfgReader& cfg)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void StepSwitch::preStep(state_vector_t& Y, double t)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void StepSwitch::stepKeysControl(double t, double dt)
{
    hod = (poz == MPOS_S  || poz == MPOS_SP || poz == MPOS_P);

    if (ctrlState.up)
    {
        poz_d += V * hs_p(MPOS_P - poz_d) * dt;
        n = 0;
    }
    if (ctrlState.up1 && poz_d < MPOS_P && n == 0)
    {
        poz += 1;
        poz_d = poz;
        n = 1;
    }
    if (ctrlState.down1 && poz_d > 0 && n == 0)
    {
        poz -= 1;
        poz_d = poz;
        n = 1;
    }
    if (ctrlState.down)
    {
        poz_d -= V * hs_p(poz_d) * dt;
        n = 0;
    }
    if (ctrlState.zero)
    {
        n = 0;
    }

    if ((getKeyState(KEY_Z) || p == 1))
    {
//        if (poz > MPOS_SP)
//        {
//            poz_d -= V1 * hs_p(poz - MPOS_SP) * dt;
//            p = hs_p(poz_d - MPOS_SP);
//        }
//        else if (poz > MPOS_S)
//        {
//            poz_d -= V1 * hs_p(poz - MPOS_S) * dt;
//            p = hs_p(poz_d - MPOS_S);
//        }
//        else if (poz > 0)
//        {
//            poz_d -= V1 * hs_p(poz) * dt;
//            p = hs_p(poz_d);
//         }

        p = 1;
        poz_d -= V1 * dt;
        if (hod || poz == 0)
            p = 0;
    }

    poz = static_cast<int>(poz_d);



    hod = (poz == MPOS_S  || poz == MPOS_SP || poz == MPOS_P);


}
