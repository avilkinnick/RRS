#include    "tep70.h"


void TEP70::stepSignalsOutput(double t, double dt)
{
    analogSignal[KM_SHTURVAL] = km->getMainShaftPos();
    analogSignal[KM_REVERSOR] = km->getReversState();

    analogSignal[BUTTON_DISEL_START] = static_cast<float>(button_disel_start);
    analogSignal[BUTTON_BRAKE_RELEASE] = static_cast<float>(!button_brake_release);
    analogSignal[BUTTON_SVISTOK] = static_cast<float>(button_svistok);
    analogSignal[BUTTON_TIFON] = static_cast<float>(button_tifon);

    analogSignal[AZV_COMMON_CONTROL] = static_cast<float>(azv_common_control.getState());
    analogSignal[AZV_UPR_TEPL] = static_cast<float>(azv_upr_tepl.getState());
    analogSignal[AZV_FUEL_PUMP] = static_cast<float>(azv_fuel_pump.getState());
    analogSignal[AZV_EDT_ON] = static_cast<float>(azv_edt_on.getState());
    analogSignal[AZV_EDT_POWER] = static_cast<float>(azv_edt_power.getState());
    analogSignal[AZV_EPT_ON] = static_cast<float>(azv_ept_on.getState());

    analogSignal[STRELKA_UR] = 0.0;
    analogSignal[STRELKA_TM] = 0.0;
    analogSignal[STRELKA_TC1] = 0.0;

    analogSignal[STRELKA_REOSTATE_CURRENT] = 0.0;
    analogSignal[STRELKA_GEN_CURRENT] = 0.0;

    analogSignal[STRELKA_WATER_TEMP] = 0.0;
    analogSignal[STRELKA_OIL_PRESS] = 0.0;

    analogSignal[LS_G] = 1.0f;

    analogSignal[TUMBLER_VOLTMETER] = static_cast<float>(tumbler_voltage.getState());
    analogSignal[TUMBLER_DISEL_STOP] = static_cast<float>(tumbler_disel_stop.getState());

    analogSignal[TUMBLER_FIELD_WEAK1] = static_cast<float>(tumbler_field_weak1.getHandlePos());
    analogSignal[TUMBLER_FIELD_WEAK2] = static_cast<float>(tumbler_field_weak2.getHandlePos());
    analogSignal[TUMBLER_WATER_ZALUZI] = static_cast<float>(tumbler_water_zaluzi.getHandlePos());
    analogSignal[TUMBLER_OIL_ZALUZI] = static_cast<float>(tumbler_oil_zaluzi.getHandlePos());

    analogSignal[STRELKA_BAT_CURRENT] = static_cast<float>(battery->getCargeCurrent() / 150.0);

    double U_bat = 0;

    if (tumbler_voltage.getState())
    {
        U_bat = ept_converter->getU_out();
    }
    else
    {
        U_bat = Ucc;
    }

    analogSignal[STRELKA_BAT_VOLTAGE] = static_cast<float>(U_bat / 150.0);

    analogSignal[STRELKA_FUEL_PRESS] = static_cast<float>(electro_fuel_pump->getFuelPressure() * Physics::g / 15.0);
    analogSignal[STRELKA_OIL_PRESS] = static_cast<float>(disel->getOilPressure() * Physics::g / 15.0);

    analogSignal[SIGLIGHT_OIL_PRESS] = getLampState(hs_p(0.1 - disel->getOilPressure()));
    analogSignal[SIGLIGHT_ZB] = getLampState(hs_p(100.0 - starter_generator->getVoltage()));

    analogSignal[STRELKA_PM] = static_cast<float>(main_reservoir->getPressure() / 1.6);

    analogSignal[RUK_367] = static_cast<float>(ubt367m->getMainHandlePos());
    analogSignal[COMB_KRAN] = static_cast<float>(ubt367m->getCombCranePos());

    analogSignal[STRELKA_TM] = static_cast<float>(pTM / 1.6);
    analogSignal[KRAN_395_RUK] = krm->getHandlePosition();

    analogSignal[STRELKA_TC1] = static_cast<float>(fwd_trolley->getBrakeCylinderPressure() / 1.0);
    analogSignal[STRELKA_TC2] = static_cast<float>(bwd_trolley->getBrakeCylinderPressure() / 1.0);

    analogSignal[STRELKA_UR] = static_cast<float>(krm->getEqReservoirPressure() / 1.0);

    analogSignal[KRAN_254_POD] = static_cast<float>(kvt->getHandleShift());
    analogSignal[KRAN_254_RUK] = static_cast<float>(kvt->getHandlePosition());

    analogSignal[KLUCH_EPK] = static_cast<float>(epk_key.getState());
    analogSignal[RB1] = static_cast<float>(button_RB1);

    analogSignal[SIGLIGHT_EPT_O] = ept_pass_control->stateReleaseLamp();
    analogSignal[SIGLIGHT_EPT_P] = ept_pass_control->stateHoldLamp();
    analogSignal[SIGLIGHT_EPT_T] = ept_pass_control->stateBrakeLamp();

    analogSignal[STRELKA_GEN_CURRENT] = static_cast<float>(I_gen / 10000.0);
    analogSignal[STRELKA_GEN_VOLTAGE] = static_cast<float>(trac_gen->getVoltage() / 1000.0);

    analogSignal[STRELKA_SPEED] = static_cast<float>(speed_meter->getArrowPos());
    analogSignal[VAL_PRSKOR1] = static_cast<float>(speed_meter->getShaftPos());
    analogSignal[VAL_PRSKOR2] = static_cast<float>(speed_meter->getShaftPos());

    analogSignal[WHEEL_1] = static_cast<float>(dir * wheel_rotation_angle[0] / 2.0 / Physics::PI);
    analogSignal[WHEEL_2] = static_cast<float>(dir * wheel_rotation_angle[1] / 2.0 / Physics::PI);
    analogSignal[WHEEL_3] = static_cast<float>(dir * wheel_rotation_angle[2] / 2.0 / Physics::PI);
    analogSignal[WHEEL_4] = static_cast<float>(dir * wheel_rotation_angle[3] / 2.0 / Physics::PI);
    analogSignal[WHEEL_5] = static_cast<float>(dir * wheel_rotation_angle[4] / 2.0 / Physics::PI);
    analogSignal[WHEEL_6] = static_cast<float>(dir * wheel_rotation_angle[5] / 2.0 / Physics::PI);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float TEP70::getLampState(double signal)
{
    bool state = azv_common_control.getState() && static_cast<bool>(signal);

    return static_cast<float>(state);
}
