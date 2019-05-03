#include    "test-loco.h"

#include    "CfgReader.h"
#include    "filesystem.h"

#include    <iostream>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TestLoco::TestLoco() : Vehicle()
  , tau(0.0)
  , delay(0.2)
  , traction_level(0.0)
  , inc_loc(false)
  , dec_loc(false)
  , inc_crane_loc(false)
  , dec_crane_loc(false)
  , crane_pos(1)
  , crane_step(0)
  , crane_motion(1.0)
  , crane_duration(0.0)
  , pz(0.0)
  , inc_brake(false)
  , dec_brake(false)
  , brake_crane_module("krm395")
  , brake_crane_config("krm395")
  , brake_crane(nullptr)  
  , brake_mech(nullptr)
  , brake_mech_module("carbrake-mech")
  , brake_mech_config("tep70bs-mech")
  , charge_press(0.5)  
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TestLoco::~TestLoco()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TestLoco::initBrakeDevices(double p0, double pTM)
{
    if (supply_reservoir != nullptr)
        supply_reservoir->setY(0, pTM);

    if (brake_crane != nullptr)
    {
        brake_crane->init(pTM);
        charge_press = p0;
        brake_crane->setChargePressure(charge_press);
    }

    if (airdist != nullptr)
        airdist->init(pTM);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TestLoco::step(double t, double dt)
{
    traction_level = Physics::cut(traction_level, 0.0, 1.0);    

    if (tau >= delay)
    {
        tau = 0.0;
        crane_pos += crane_step;
    }

    if (brake_mech != nullptr)
    {
        brake_mech->setVelocity(velocity);        

        brake_mech->setAirFlow(repiter->getBrakeCylAirFlow());
        brake_mech->step(t, dt);
    }

    for (size_t i = 1; i < Q_a.size(); i++)
    {
        double torque = traction_level * traction_char(velocity) * wheel_diameter / num_axis / 2.0;
        double brakeTorque = brake_mech->getBrakeTorque();
        Q_a[i] = torque;
        Q_r[i] = brakeTorque;
    }

    if (brake_crane != nullptr)
    {
        brake_crane->setChargePressure(charge_press);
        brake_crane->setFeedLinePressure(0.9);
        p0 = brake_crane->getBrakePipeInitPressure();
        brake_crane->setBrakePipePressure(pTM);
        brake_crane->setPosition(crane_pos);
        brake_crane->step(t, dt);
    }

    if ( supply_reservoir != nullptr )
    {
        supply_reservoir->setAirFlow(airdist->getAirSupplyFlow());
        supply_reservoir->step(t, dt);
    }

    if ( airdist != nullptr)
    {
        airdist->setBrakepipePressure(pTM);
        //airdist->setBrakeCylinderPressure(repiter->getWorkPressure());
        airdist->setBrakeCylinderPressure(zpk->getPressure1());
        airdist->setAirSupplyPressure(supply_reservoir->getPressure());

        auxRate = airdist->getAuxRate();

        airdist->step(t, dt);
    }

    repiter->setPipelinePressure(0.9);
    repiter->setBrakeCylPressure(brake_mech->getBrakeCylinderPressure());
    //repiter->setWorkAirFlow(airdist->getBrakeCylinderAirFlow());
    repiter->setWorkAirFlow(zpk->getOutputFlow());

    repiter->step(t, dt);

    zpk->setInputFlow2(0.0);
    zpk->setInputFlow1(airdist->getBrakeCylinderAirFlow());
    zpk->setOutputPressure(repiter->getWorkPressure());

    zpk->step(t, dt);

    emit soundSetPitch("Disel", 1.0f + static_cast<float>(traction_level) / 1.0f);

    DebugMsg = QString("Время: %1 Шаг: %5 Коорд.: %2 Скор.: %3 Тяга: %4 УР: %6 ТМ: %7 ТЦ: %8 КрМ: %9 ЗР: %10 v2: %11")
            .arg(t, 7, 'f', 1)
            .arg(railway_coord, 10, 'f', 2)
            .arg(velocity * Physics::kmh, 6, 'f', 1)
            .arg(traction_level, 3, 'f', 1)
            .arg(dt, 8, 'f', 6)
            .arg(brake_crane->getEqReservoirPressure(), 4, 'f', 2)
            .arg(brake_crane->getBrakePipeInitPressure(), 4, 'f', 2)
            .arg(brake_mech->getBrakeCylinderPressure(), 4, 'f', 2)
            .arg(brake_crane->getPositionName(crane_pos), 4)
            .arg(supply_reservoir->getPressure(), 4, 'f', 2)
            .arg(airdist->getY(5), 10, 'f', 6);

    DebugMsg += airdist->getDebugMsg();

    tau += dt;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double TestLoco::traction_char(double v)
{
    double max_traction = 280e3;

    double vn = 81.0 / Physics::kmh;

    if (abs(v) <= vn)
        return max_traction;
    else
        return max_traction * vn / abs(v);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TestLoco::initialization()
{
    FileSystem &fs = FileSystem::getInstance();
    QString modules_dir(fs.getModulesDir().c_str());
    brake_crane = loadBrakeCrane(modules_dir + fs.separator() + brake_crane_module);
    brake_mech = loadBrakeMech(modules_dir + fs.separator() + brake_mech_module);
    supply_reservoir = new Reservoir(0.078);

    airdist = loadAirDistributor(modules_dir + fs.separator() + airdist_module);

    repiter = new PneumoReley();
    repiter->read_config("rd304");

    zpk = new SwitchingValve();
    zpk->read_config("zpk");

    if (brake_crane != nullptr)
    {
        brake_crane->read_config(brake_crane_config);

        brake_mech->read_config(brake_mech_config);
        brake_mech->setEffFricRadius(wheel_diameter / 2.0);
        brake_mech->setWheelDiameter(wheel_diameter);

        airdist->read_config(airdist_config);

        connect(brake_crane, &BrakeCrane::soundSetVolume, this, &TestLoco::soundSetVolume);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TestLoco::loadConfig(QString cfg_path)
{
    CfgReader cfg;

    if (cfg.load(cfg_path))
    {
        QString secName = "Vehicle";

        cfg.getString(secName, "BrakeCraneModule", brake_crane_module);
        cfg.getString(secName, "BrakeCraneConfig", brake_crane_config);

        cfg.getString(secName, "BrakeMechModule", brake_mech_module);
        cfg.getString(secName, "BrakeMechCinfig", brake_mech_config);

        cfg.getString(secName, "AirDistModule", airdist_module);
        cfg.getString(secName, "AirDistConfig", airdist_config);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TestLoco::keyProcess()
{
    incTracTrig.process(keys[KEY_A], traction_level);
    decTracTrig.process(keys[KEY_D], traction_level);

    //incBrakeCrane.process(keys[KEY_Rightbracket], crane_pos);
    //decBrakeCrane.process(keys[KEY_Leftbracket], crane_pos);

    crane_step = 0;

    if (keys[KEY_Quote])
    {
        crane_step = 1;
    }

    if (keys[KEY_Semicolon])
    {
        crane_step = -1;
    }

    if (keys[KEY_1])
    {
        crane_pos = 0;
    }

    if (keys[KEY_2])
    {
        crane_pos = 1;
    }

    if (keys[KEY_3])
    {
        crane_pos = 2;
    }

    if (keys[KEY_4])
    {
        crane_pos = 3;
    }

    if (keys[KEY_5])
    {
        crane_pos = 4;
    }

    if (keys[KEY_6])
    {
        crane_pos = 5;
    }

    if (keys[KEY_7])
    {
        crane_pos = 6;
    }

    incChargePress.process(keys[KEY_H], charge_press);
    decChargePress.process(keys[KEY_J], charge_press);

    if (keys[KEY_F])
    {
        emit soundSetVolume("Svistok", 100);
    }
    else
    {
        emit soundSetVolume("Svistok", 0);
    }

    if (keys[KEY_G])
    {
        emit soundSetVolume("Tifon", 100);
    }
    else
    {
        emit soundSetVolume("Tifon", 0);
    }

    analogSignal[0] = static_cast<float>(traction_level);
    analogSignal[1] = static_cast<float>(brake_crane->getBrakePipeInitPressure());
    analogSignal[2] = static_cast<float>(brake_crane->getEqReservoirPressure());
    analogSignal[3] = crane_pos;
    analogSignal[4] = static_cast<float>(pTM);

    analogSignal[20] = brake_crane->getHandlePosition(crane_pos);
    analogSignal[21] = static_cast<float>(pTM / 1.0);
    analogSignal[22] = static_cast<float>(brake_crane->getEqReservoirPressure() / 1.0);
    analogSignal[23] = static_cast<float>(0.9 / 1.6);
    analogSignal[24] = static_cast<float>(repiter->getWorkPressure() / 1.0);
    analogSignal[25] = static_cast<float>(brake_mech->getBrakeCylinderPressure() / 1.0);
    analogSignal[26] = static_cast<float>(abs(velocity) * Physics::kmh / 220.0);
    analogSignal[27] = static_cast<float>(abs(velocity) * Physics::kmh / 150.0);
    analogSignal[28] = static_cast<float>(traction_level);
}

void TestLoco::stepper(int &value, int duration, double dt)
{
    double tmp = static_cast<double>(value);
    tmp += duration * dt;
    value = static_cast<int>(tmp);
}

GET_VEHICLE(TestLoco)
