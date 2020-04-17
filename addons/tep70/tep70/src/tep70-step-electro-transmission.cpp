#include    "tep70.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TEP70::stepElectroTransmission(double t, double dt)
{
    // Ток, потребляемый от главного генератора
    I_gen = 0.0;

    // Состояние цепи поездных контакторов
    bool is_KP_on = azv_upr_tepl.getState() && brake_switcher->isTraction();

    tracForce = 0;

    // Состояние последовательной цепи размыкающих контактов КП1 - КП7
    bool is_KP1_KP7_off = true;

    // Состояние последовательной цепи замыкающих контактор КП1 - КП6
    bool is_KP1_KP6_on = true;

    for (size_t i = 0; i < motor.size(); ++i)
    {
        kp[i]->setVoltage(Ucc * static_cast<double>(is_KP_on));
        kp[i]->step(t, dt);

        motor[i]->setAncorVoltage(trac_gen->getVoltage() * static_cast<double>(kp[i]->getContactState(0)));
        motor[i]->setOmega(ip * wheel_omega[i]);

        // Ослабление возбуждения
        double beta = 1.0 - 0.58 * static_cast<double>(ksh1->getContactState(0))
                          - 0.08 * static_cast<double>(ksh2->getContactState(0));

        motor[i]->setFieldWeak(beta);

        motor[i]->setReversSate(reversor->getSatate());

        motor[i]->setMode(brake_switcher->getState());

        motor[i]->step(t, dt);

        Q_a[i+1] = motor[i]->getTorque() * ip;

        tracForce += Q_a[i+1] * 2 / wheel_diameter;

        I_gen += motor[i]->getAncorCurrent();

        is_KP1_KP7_off = is_KP1_KP7_off && kp[i]->getContactState(1);
        is_KP1_KP6_on = is_KP1_KP6_on && kp[i]->getContactState(2);
    }

    is_KP1_KP7_off = is_KP1_KP7_off && kp[6]->getContactState(1);

    // Состояние цепи контактора возбуждения возбудителя
    bool is_KVV_on = azv_upr_tepl.getState() && is_KP1_KP6_on;

    kvv->setVoltage(Ucc * static_cast<double>(is_KVV_on));
    kvv->step(t, dt);

    // Состояние цепи контактора возбуждения генератора
    bool is_KVG_on = azv_upr_tepl.getState() && is_KP1_KP6_on;

    kvg->setVoltage(Ucc * static_cast<double>(is_KVG_on));
    kvg->step(t, dt);

    // Состояние цепи обмотки возбуждения возбудителя
    bool is_FGF_on = kontaktor_starter->getContactState(1) ||
                     kvv->getContactState(0);

    field_gen->setFieldVoltage(Ucc * static_cast<double>(is_FGF_on));
    field_gen->setLoadCurrent(trac_gen->getFieldCurrent());
    field_gen->setOmega(disel->getStarterOmega());
    field_gen->step(t, dt);    

    // Состояние цепи возбуждения главного генератора
    bool is_TGF_on = kvg->getContactState(0);

    trac_gen->setFieldVoltage(field_reg->getFieldVoltage() * static_cast<double>(is_TGF_on));
    trac_gen->setLoadCurrent(I_gen);
    trac_gen->setOmega(disel->getOmega());
    trac_gen->step(t, dt);

    field_reg->setFGVoltage(field_gen->getVoltage());
    field_reg->setOmega(disel->getOmega());
    field_reg->setGenVoltage(trac_gen->getVoltage());
    field_reg->setGenCurrent(I_gen);
    field_reg->setKMPosition(km->getPositionNumber());
    field_reg->step(t, dt);

    speed_meter->setWheelDiameter(wheel_diameter);
    speed_meter->setOmega(wheel_omega[0]);
    speed_meter->step(t, dt);

    // Цепь реле РУ1
    bool is_RU1_on = azv_upr_tepl.getState() && km->is12orMore();

    ru1->setVoltage(Ucc * static_cast<double>(is_RU1_on));
    ru1->step(t, dt);

    // Цепь контактора КШ1
    bool is_KSH1_on = ( (tumbler_field_weak1.getState() == 0) &&
                      ru1->getContactState(0) ) ||
                      ( (tumbler_field_weak1.getState() == 2) && (abs(wheel_omega[0]) >= 32.8) && azv_upr_tepl.getState() );

    ksh1->setVoltage(Ucc * static_cast<double>(is_KSH1_on));
    ksh1->step(t, dt);

    // Цепь контактора КШ1
    bool is_KSH2_on = ( (tumbler_field_weak2.getState() == 0) &&
                      ru1->getContactState(1) ) ||
                      ( (tumbler_field_weak2.getState() == 2) && (abs(wheel_omega[0]) >= 43.7) && azv_upr_tepl.getState() );

    ksh2->setVoltage(Ucc * static_cast<double>(is_KSH2_on));
    ksh2->step(t, dt);


    // Цепь вентиля реврсора вперед
    bool is_Revers_Forward = azv_upr_tepl.getState() && km->isForward();

    // Цепь вентиля реверсора назад
    bool is_Revers_Backward = azv_upr_tepl.getState() && km->isBackward();

    reversor->setForwardValveState(is_Revers_Forward);
    reversor->setBackwardValveState(is_Revers_Backward);
    reversor->step(t, dt);

    // Цепь вентиля "Тяга" тормозного переключателя
    bool is_TRAC_on = azv_upr_tepl.getState() &&
                     (is_KP1_KP7_off || brake_switcher->isTraction()) &&
                     km->isNoZero();

    brake_switcher->setTracValeState(is_TRAC_on);
    brake_switcher->setBrakeValveState(false);
    brake_switcher->step(t, dt);
}
