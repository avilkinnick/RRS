#include    "vl60.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepOtherEquipment(double t, double dt)
{
    speed_meter->setOmega(wheel_omega[TED1]);
    speed_meter->step(t, dt);

    horn->setControl(keys);
    horn->step(t, dt);

    debugPrint(t);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepTapSound()
{
    double speed = abs(this->velocity) * 3.6;

    for (int i = 0; i < tap_sounds.count(); ++i)
    {
        emit volumeCurveStep(tap_sounds[i], static_cast<float>(speed));
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::step(double t, double dt)
{
    stepPantographsControl(t, dt);

    stepMainSwitchControl(t, dt);

    stepTracTransformer(t, dt);

    stepPhaseSplitter(t, dt);

    stepMotorFans(t, dt);

    stepMotorCompressor(t, dt);

    stepBrakeControl(t, dt);

    stepTrolleysBrakeMech(t, dt);

    stepAirDistributors(t, dt);

    stepSignalsOutput();

    stepTractionControl(t, dt);

    stepLineContactors(t, dt);

    stepOtherEquipment(t, dt);

    stepEPT(t, dt);

    stepTapSound();

    autoStartTimer->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::slotAutoStart()
{
    if (start_count < triggers.size())
    {
        triggers[start_count]->set();

        if (!pantographs[0]->isUp() && !pantographs[1]->isUp() &&
                (triggers[start_count] == &gv_tumbler))
            return;

        if (!static_cast<bool>(main_switch->getLampState()))
            gv_return_tumbler.reset();

        start_count++;
    }
    else
    {
        autoStartTimer->stop();
        controller->setReversPos(REVERS_FORWARD);
        start_count = 0;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepPantographsControl(double t, double dt)
{
    pantographs[0]->setState(pant1_tumbler.getState() && pants_tumbler.getState());
    pantographs[1]->setState(pant2_tumbler.getState() && pants_tumbler.getState());

    for (auto pant : pantographs)
    {
        // Задаем текущее напряжение КС (пока что через константу)
        pant->setUks(Uks);
        // Моделируем работу токоприемников
        pant->step(t, dt);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepMainSwitchControl(double t, double dt)
{
    // Подаем на вход напряжение с крышевой шины, на которую включены
    // оба токоприемника
    main_switch->setU_in(max(pantographs[0]->getUout(), pantographs[1]->getUout()));

    // Задаем состояние органов управления ГВ
    main_switch->setState(gv_tumbler.getState());
    main_switch->setReturn(gv_return_tumbler.getState());

    // Подаем питание на удерживающую катушку ГВ
    main_switch->setHoldingCoilState(getHoldingCoilState());

    gauge_KV_ks->setInput(main_switch->getU_out() / 30000.0);

    // Моделируем работу ГВ
    main_switch->step(t, dt);

    gauge_KV_ks->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepTracTransformer(double t, double dt)
{
    // Задаем напряжение на первичной обмотке (с выхода ГВ)
    trac_trans->setU1(main_switch->getU_out());
    trac_trans->setPosition(main_controller->getPosition());

    trac_trans->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepPhaseSplitter(double t, double dt)
{
    double U_power = trac_trans->getU_sn() * static_cast<double>(fr_tumbler.getState());
    phase_spliter->setU_power(U_power);

    phase_spliter->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepMotorFans(double t, double dt)
{
    for (size_t i = 0; i < NUM_MOTOR_FANS; ++i)
    {
        MotorFan *mf = motor_fans[i];
        mf->setU_power(phase_spliter->getU_out() * static_cast<double>(mv_tumblers[i].getState()));
        mf->step(t, dt);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepMotorCompressor(double t, double dt)
{
    //double k_flow = 5e-3;
    //main_reservoir->setFlowCoeff(k_flow);
    main_reservoir->setAirFlow(motor_compressor->getAirFlow());
    main_reservoir->step(t, dt);

    motor_compressor->setExternalPressure(main_reservoir->getPressure());
    motor_compressor->setU_power(phase_spliter->getU_out() * static_cast<double>(mk_tumbler.getState()) * press_reg->getState());
    motor_compressor->step(t, dt);

    press_reg->setPressure(main_reservoir->getPressure());
    press_reg->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepBrakeControl(double t, double dt)
{
    // Подключаем к УБТ трубопровод от ГР
    ubt->setLocoFLpressure(main_reservoir->getPressure());
    // Подключаем к УБТ трубопровод ТМ от КрМ
    ubt->setCraneTMpressure(brake_crane->getBrakePipeInitPressure());
    ubt->setControl(keys);
    // Задаем давление в начале ТМ
    p0 = ubt->getLocoTMpressure();
    ubt->step(t, dt);

    brake_crane->setFeedLinePressure(ubt->getCraneFLpressure());
    brake_crane->setChargePressure(charge_press);
    brake_crane->setBrakePipePressure(pTM);
    brake_crane->setControl(keys);
    brake_crane->step(t, dt);

    loco_crane->setFeedlinePressure(ubt->getCraneFLpressure());
    loco_crane->setBrakeCylinderPressure(switch_valve->getPressure2());
    loco_crane->setAirDistributorFlow(0.0);
    loco_crane->setControl(keys);
    loco_crane->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepTrolleysBrakeMech(double t, double dt)
{
    switch_valve->setInputFlow1(electroAirDist->getQbc_out());
    switch_valve->setInputFlow2(loco_crane->getBrakeCylinderFlow());
    switch_valve->setOutputPressure(pneumo_splitter->getP_in());
    switch_valve->step(t, dt);

    // Тройник подключен к ЗПК
    pneumo_splitter->setQ_in(switch_valve->getOutputFlow());
    pneumo_splitter->setP_out1(pneumo_relay->getWorkPressure());
    pneumo_splitter->setP_out2(trolley_mech[TROLLEY_BWD]->getBrakeCylinderPressure());
    pneumo_splitter->step(t, dt);

    pneumo_relay->setPipelinePressure(main_reservoir->getPressure());
    pneumo_relay->setWorkAirFlow(pneumo_splitter->getQ_out1());
    pneumo_relay->setBrakeCylPressure(trolley_mech[TROLLEY_FWD]->getBrakeCylinderPressure());
    pneumo_relay->step(t, dt);

    // Передняя тележка наполняется через реле давления 304
    trolley_mech[TROLLEY_FWD]->setAirFlow(pneumo_relay->getBrakeCylAirFlow());
    trolley_mech[TROLLEY_FWD]->setVelocity(velocity);
    trolley_mech[TROLLEY_FWD]->step(t, dt);

    // Задняя тележка подключена через тройник от ЗПК
    trolley_mech[TROLLEY_BWD]->setAirFlow(pneumo_splitter->getQ_out2());
    trolley_mech[TROLLEY_FWD]->setVelocity(velocity);
    trolley_mech[TROLLEY_BWD]->step(t, dt);

    Q_r[1] = trolley_mech[TROLLEY_FWD]->getBrakeTorque();
    Q_r[2] = trolley_mech[TROLLEY_FWD]->getBrakeTorque();
    Q_r[3] = trolley_mech[TROLLEY_FWD]->getBrakeTorque();

    Q_r[4] = trolley_mech[TROLLEY_BWD]->getBrakeTorque();
    Q_r[5] = trolley_mech[TROLLEY_BWD]->getBrakeTorque();
    Q_r[6] = trolley_mech[TROLLEY_BWD]->getBrakeTorque();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepAirDistributors(double t, double dt)
{
    supply_reservoir->setAirFlow(electroAirDist->getOutputSupplyReservoirFlow());
    supply_reservoir->step(t, dt);

    air_disr->setBrakeCylinderPressure(electroAirDist->getPbc_out());
    air_disr->setAirSupplyPressure(electroAirDist->getSupplyReservoirPressure());
    air_disr->setBrakepipePressure(pTM);
    auxRate = air_disr->getAuxRate();
    air_disr->step(t, dt);

    electroAirDist->setControlLine(ept_control[0]);
    electroAirDist->setQbc_in(air_disr->getBrakeCylinderAirFlow());
    electroAirDist->setPbc_in(switch_valve->getPressure1());
    electroAirDist->setInputSupplyReservoirFlow(air_disr->getAirSupplyFlow());
    electroAirDist->setSupplyReservoirPressure(supply_reservoir->getPressure());
    electroAirDist->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepTractionControl(double t, double dt)
{
    controller->setControl(keys);
    controller->step(t, dt);

    main_controller->enable(cu_tumbler.getState() && static_cast<bool>(ubt->getState()));
    main_controller->setKMstate(controller->getState());
    main_controller->step(t, dt);

    gauge_KV_motors->setInput(vu[VU1]->getU_out());
    gauge_KV_motors->step(t, dt);

    motor[TED1]->setU(vu[VU1]->getU_out() * static_cast<double>(line_contactor[TED1].getState()));
    motor[TED2]->setU(vu[VU1]->getU_out() * static_cast<double>(line_contactor[TED2].getState()));
    motor[TED3]->setU(vu[VU1]->getU_out() * static_cast<double>(line_contactor[TED3].getState()));

    motor[TED4]->setU(vu[VU2]->getU_out() * static_cast<double>(line_contactor[TED4].getState()));
    motor[TED5]->setU(vu[VU2]->getU_out() * static_cast<double>(line_contactor[TED5].getState()));
    motor[TED6]->setU(vu[VU2]->getU_out() * static_cast<double>(line_contactor[TED6].getState()));

    km_state_t km_state = controller->getState();

    double I_vu = 0;

    for (size_t i = 0; i < motor.size(); ++i)
    {
        motor[i]->setDirection(km_state.revers_ref_state);
        motor[i]->setOmega(ip * wheel_omega[i]);
        motor[i]->setBetaStep(km_state.field_loosen_pos);
        Q_a[i+1] = motor[i]->getTorque() * ip;
        motor[i]->step(t, dt);

        I_vu += motor[i]->getIa();

        overload_relay[i]->setCurrent(motor[i]->getIa());
        overload_relay[i]->step(t, dt);
    }

    for (auto v : vu)
    {
        v->setI_out(I_vu);
        v->setU_in(trac_trans->getTracVoltage());
        v->step(t, dt);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepLineContactors(double t, double dt)
{
    Q_UNUSED(t)
    Q_UNUSED(dt)

    km_state_t km_state = controller->getState();

    bool motor_fans_state = true;

    for (auto mv: motor_fans)
    {
        motor_fans_state = motor_fans_state && !static_cast<bool>(mv->isNoReady());
    }

    bool lk_state = !km_state.pos_state[POS_BV] &&
                    !km_state.pos_state[POS_ZERO] &&
                    motor_fans_state && main_controller->isReady();

    lineContactorsControl(lk_state);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::lineContactorsControl(bool state)
{
    for (size_t i = 0; i < line_contactor.size(); ++i)
    {
        if (state)
            line_contactor[i].set();
        else
            line_contactor[i].reset();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float VL60pk::isLineContactorsOff()
{
    bool state = true;

    for (size_t i = 0; i < line_contactor.size(); ++i)
    {
        state = state && line_contactor[i].getState();
    }

    return 1.0f - static_cast<float>(state);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double VL60pk::getTractionForce()
{
    double ip = 2.73;

    double sumTorque = 0;

    for (auto m : motor)
    {
        sumTorque += m->getTorque();
    }

    double force = sumTorque * ip * 2.0 / wheel_diameter;

    return force;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool VL60pk::getHoldingCoilState() const
{
    km_state_t km_state = controller->getState();

    bool no_overload = true;

    for (auto ov_relay : overload_relay)
    {
        no_overload = no_overload && (!static_cast<bool>(ov_relay->getState()));
    }

    bool state = !km_state.pos_state[POS_BV] && no_overload;

    return state;
}
