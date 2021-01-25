#include    "vl60.h"

#include    "filesystem.h"

#include    <QDir>


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initBrakeDevices(double p0, double pTM, double pFL)
{
    main_reservoir->setY(0, pFL);
    charge_press = p0;

    brake_crane->setChargePressure(charge_press);
    brake_crane->init(pTM, pFL);
    loco_crane->init(pTM, pFL);
    air_disr->init(pTM, pFL);

    load_brakes_config(config_dir + QDir::separator() + "brakes-init.xml");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initPantographs()
{
    QString pant_cfg_path = config_dir + QDir::separator() + "pantograph";

    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i)
    {
        pantographs[i] = new Pantograph();
        pantographs[i]->read_custom_config(pant_cfg_path);
        connect(pantographs[i], &Pantograph::soundPlay, this, &VL60pk::soundPlay);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initHighVoltageScheme()
{
    QString gv_cfg_path = config_dir + QDir::separator() + "main-switch";

    main_switch = new ProtectiveDevice();
    main_switch->read_custom_config(gv_cfg_path);
    connect(main_switch, &ProtectiveDevice::soundPlay, this, &VL60pk::soundPlay);

    gauge_KV_ks = new Oscillator();
    gauge_KV_ks->read_config("oscillator");

    trac_trans = new TracTransformer();
    trac_trans->read_custom_config(config_dir + QDir::separator() + "trac-transformer");
    connect(trac_trans, &TracTransformer::soundSetVolume, this, &VL60pk::soundSetVolume);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initSupplyMachines()
{
    phase_spliter = new PhaseSplitter();
    connect(phase_spliter, &PhaseSplitter::soundPlay, this, &VL60pk::soundPlay);
    connect(phase_spliter, &PhaseSplitter::soundStop, this, &VL60pk::soundStop);

    for (size_t i = 0; i < motor_fans.size(); ++i)
    {
        motor_fans[i] = new MotorFan(i + 1);
        //connect(motor_fans[i], &MotorFan::soundSetPitch, this, &VL60pk::soundSetPitch);
        connect(motor_fans[i], &MotorFan::soundPlay, this, &VL60pk::soundPlay);
        connect(motor_fans[i], &MotorFan::soundStop, this, &VL60pk::soundStop);
    }

    main_reservoir = new Reservoir(static_cast<double>(MAIN_RESERVOIR_VOLUME) / 1000.0);

    QString mk_cfg_path = config_dir + QDir::separator() + "motor-compressor.xml";
    motor_compressor = new MotorCompressor(mk_cfg_path);
    connect(motor_compressor, &MotorCompressor::soundSetPitch, this, &VL60pk::soundSetPitch);

    press_reg = new PressureRegulator();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initBrakeControls(QString modules_dir)
{
    ubt = new BrakeLock();
    ubt->read_config("ubt367m");
    connect(ubt, &BrakeLock::soundPlay, this, &VL60pk::soundPlay);

    brake_crane = loadBrakeCrane(modules_dir + QDir::separator() + "krm395");
    brake_crane->read_config("krm395");
    connect(brake_crane, &BrakeCrane::soundPlay, this, &VL60pk::soundPlay);
    connect(brake_crane, &BrakeCrane::soundSetVolume, this, &VL60pk::soundSetVolume);

    loco_crane = loadLocoCrane(modules_dir + QDir::separator() + "kvt254");
    loco_crane->read_config("kvt254");
    connect(loco_crane, &LocoCrane::soundPlay, this, &VL60pk::soundPlay);
    connect(loco_crane, &LocoCrane::soundStop, this, &VL60pk::soundStop);
    connect(loco_crane, &LocoCrane::soundSetVolume, this, &VL60pk::soundSetVolume);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initBrakeMechanics()
{
    trolley_mech[TROLLEY_FWD] = new TrolleyBrakeMech(config_dir +
                                           QDir::separator() +
                                           "fwd-trolley-brake-mech.xml");

    trolley_mech[TROLLEY_BWD] = new TrolleyBrakeMech(config_dir +
                                           QDir::separator() +
                                           "bwd-trolley-brake-mech.xml");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initBrakeEquipment(QString modules_dir)
{
    switch_valve = new SwitchingValve();
    switch_valve->read_config("zpk");

    pneumo_relay = new PneumoReley();
    pneumo_relay->read_config("rd304");

    pneumo_splitter = new PneumoSplitter();
    pneumo_splitter->read_config("pneumo-splitter");

    supply_reservoir = new Reservoir(0.078);

    air_disr = loadAirDistributor(modules_dir + QDir::separator() + "vr242");
    air_disr->read_config("vr242");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initTractionControl()
{
    controller = new ControllerKME_60_044();
    connect(controller, &ControllerKME_60_044::soundPlay, this, &VL60pk::soundPlay);

    main_controller = new EKG_8G();
    main_controller->read_custom_config(config_dir + QDir::separator() + "ekg-8g");
    connect(main_controller, &EKG_8G::soundPlay, this, &VL60pk::soundPlay);

    for (size_t i = 0; i < vu.size(); ++i)
    {
        vu[i] = new Rectifier();
        vu[i]->read_custom_config(config_dir + QDir::separator() + "VU");
    }

    gauge_KV_motors = new Oscillator();
    gauge_KV_motors->read_custom_config(config_dir + QDir::separator() + "KV1-osc");

    for (size_t i = 0; i < motor.size(); ++i)
    {
        motor[i] = new DCMotor();
        motor[i]->setCustomConfigDir(config_dir);
        motor[i]->read_custom_config(config_dir + QDir::separator() + "HB-412K");
        connect(motor[i], &DCMotor::soundSetPitch, this, &VL60pk::soundSetPitch);
        connect(motor[i], &DCMotor::soundSetVolume, this, &VL60pk::soundSetVolume);

        overload_relay[i] = new OverloadRelay();
        overload_relay[i]->read_custom_config(config_dir + QDir::separator() + "PT-140A");
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initOtherEquipment()
{
    speed_meter = new SL2M();
    speed_meter->setWheelDiameter(wheel_diameter);
    speed_meter->read_custom_config(config_dir + QDir::separator() + "3SL-2M");
    connect(speed_meter, &SL2M::soundSetVolume, this, &VL60pk::soundSetVolume);

    horn = new TrainHorn();
    connect(horn, &TrainHorn::soundPlay, this, &VL60pk::soundPlay);
    connect(horn, &TrainHorn::soundSetVolume, this, &VL60pk::soundSetVolume);
    connect(horn, &TrainHorn::soundStop, this, &VL60pk::soundStop);

    //reg = new Registrator("brakes");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initTriggers()
{
    triggers.push_back(&pants_tumbler);
    triggers.push_back(&pant2_tumbler);
    triggers.push_back(&gv_tumbler);
    triggers.push_back(&gv_return_tumbler);
    triggers.push_back(&fr_tumbler);
    triggers.push_back(&mk_tumbler);

    for (size_t i = 0; i < mv_tumblers.size(); ++i)
        triggers.push_back(&mv_tumblers[i]);

    triggers.push_back(&cu_tumbler);

    autoStartTimer = new Timer(0.5);
    connect(autoStartTimer, &Timer::process, this, &VL60pk::slotAutoStart);
    start_count = 0;
}

void VL60pk::initTapSounds() {
    QString f_p = "tap_";

    tap_sounds << (f_p + "5-10");
    tap_sounds << (f_p + "10-20");
    tap_sounds << (f_p + "20-30");
    tap_sounds << (f_p + "30-40");
    tap_sounds << (f_p + "40-50");
    tap_sounds << (f_p + "50-60");
    tap_sounds << (f_p + "60-70");
    tap_sounds << (f_p + "70-80");
    tap_sounds << (f_p + "80-90");
    tap_sounds << (f_p + "90-100");
    tap_sounds << (f_p + "100-110");
    tap_sounds << (f_p + "110-~");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::initialization()
{
    FileSystem &fs = FileSystem::getInstance();
    QString modules_dir = QString(fs.getModulesDir().c_str());

    Uks = WIRE_VOLTAGE;
    current_kind = 1;

    initPantographs();

    initHighVoltageScheme();

    initSupplyMachines();

    initBrakeControls(modules_dir);

    initBrakeMechanics();

    initBrakeEquipment(modules_dir);

    initTractionControl();

    initOtherEquipment();

    initTriggers();

    initEPT(modules_dir);

    initTapSounds();
}
