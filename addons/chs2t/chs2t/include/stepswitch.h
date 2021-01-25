#ifndef     STEPSWITCH_H
#define     STEPSWITCH_H

#include    "device.h"
#include    "km-21kr2-state.h"

#include   "ampermeters-state.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class StepSwitch : public Device
{
public:

    ///Конструктор
    StepSwitch(QObject *parent = Q_NULLPTR);

    ///Деструктор
    ~StepSwitch();

    void setCtrlState(ControllerState ctrlState) { this->ctrlState = ctrlState; }

    void setDropPosition(bool value) { dropPosition = value; }

    int getPoz() { return poz; }

    double getSchemeState() const { return static_cast<double>(poz != 0); }

    bool getHod() { return hod; }

    int getFieldStep() const { return fieldStep; }

    int getReverseState() const { return reverseState; }

    void setDropPositionsWithZ(bool value) { dropPositionsWithZ = value; }

    bool getDropPositionsWithZ() { return dropPositionsWithZ; }

    bool isZero() const { return poz == 0; }

    bool isSeries() const { return (poz > 0 ) && (poz <= MPOS_S) && !hod; }

    bool isSeriesParallel() const { return (poz > MPOS_S ) && (poz <= MPOS_SP) &&!hod; }

    bool isParallel() const { return (poz > MPOS_SP ) && !hod; }

    ampermeters_state_t getAmpermetersState();

private:

    enum
    {
        MPOS_S = 20,
        MPOS_SP = 33,
        MPOS_P = 42
    };

    double V;

    double poz_d;

    int poz;

    int fieldStep;
    int reverseState;

    bool up;
    bool up1;
    bool zero;
    bool down1;
    bool down;

    bool onePositionIsChanged;
    bool dropPositionsWithZ;
    bool ableToGainPositions;

    int prevPos;
    int prevPos2;

    bool dropPosition;

    bool hod;

    ControllerState ctrlState;
    bool ableToChangeOnePosition;

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);

    void preStep(state_vector_t &Y, double t);

    void stepDiscrete(double t, double dt);

    void changeOnePosition(int dir);
};


#endif // STEPSWITCH_H
