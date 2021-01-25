#include    "vl60.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VL60pk::stepEPT(double t, double dt)
{
    ept_converter->setU_bat(U_bat);
    ept_converter->setI_out(ept_current[0]);
    ept_converter->step(t, dt);

    ept_pass_control->setU(ept_converter->getU_out() * static_cast<double>(ept_switch.getState()));
    ept_pass_control->setHoldState(brake_crane->isHold());
    ept_pass_control->setBrakeState(brake_crane->isBrake());
    ept_pass_control->step(t, dt);

    ept_control[0] = ept_pass_control->getControlSignal();

    if (prev_vehicle != nullptr)
    {
        ept_control[0] = prev_vehicle->getEPTControl(0);
    }

    ept_current[0] = 0;

    if (next_vehicle != nullptr)
    {
        ept_current[0] += next_vehicle->getEPTCurrent(0);
        next_vehicle->setEPTControl(0, ept_control[0]);
    }

    ept_current[0] += electroAirDist->getValveState(0) + electroAirDist->getValveState(1);
}
