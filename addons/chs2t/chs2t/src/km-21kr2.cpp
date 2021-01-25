#include "km-21kr2.h"



//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
Km21KR2::Km21KR2(QObject* parent) : Device(parent)
  , k01(false)
  , k02(false)

  , k21(false)
  , k22(false)
  , k23(false)
  , k25(false)

  , k31(false)
  , k32(false)
  , k33(false)


  , autoSet(false)
  , autoReset(false)
  , lastControllerPositionIsZero(false)
  , reverseIsPressedOneTime(false)
  , hod(false)
  , reverseState(0)
  , mainShaftPos(0.0)
  , fieldWeakShaft(0.0)
  , mainShaftHeight(0.0)
  , is_inc(true)
  , is_dec(true)
{
    y.resize(1);
}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
Km21KR2::~Km21KR2()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::ode_system(const state_vector_t& Y, state_vector_t& dYdt, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)
    Q_UNUSED(dYdt)

    dYdt[0] = (mainShaftHeight - Y[0]) / 0.1;
}

//------------------------------------------------------------------------------
// Загрузка данных из конфигурационного файла
//------------------------------------------------------------------------------
void Km21KR2::load_config(CfgReader& cfg)
{
    Q_UNUSED(cfg)

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::preStep(state_vector_t& Y, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)

    addSignalsInControllerState();

    k01 = (reverseState == 1);

    k02 = (reverseState == -1);

    k25 = (mainShaftPos == -10 || mainShaftPos == -5 || mainShaftPos == 0);
    k21 = (mainShaftPos == -10 || mainShaftPos == 0  || mainShaftPos == 4);
    k22 = (mainShaftPos == 0   || mainShaftPos == 2  || mainShaftPos == 4);
    k23 = (mainShaftPos == 2   || mainShaftPos == 4);

    k31 = (fieldWeakShaft == 2 || fieldWeakShaft == 8 || fieldWeakShaft == 10 );
    k32 = (fieldWeakShaft == 4 || fieldWeakShaft == 8);
    k33 = (fieldWeakShaft == 6 || fieldWeakShaft == 10);

    if ((k01 != controlState.k01) || (k02 != controlState.k02))
        emit soundPlay("revers");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::stepKeysControl(double t, double dt)
{
    Q_UNUSED(t)
    Q_UNUSED(dt)

    if (!reverseIsPressedOneTime && (TO_INT(mainShaftPos) == 0) && (TO_INT(fieldWeakShaft) == 0))
        reverseState += ((getKeyState(KEY_W) && reverseState != 1) -
                         (getKeyState(KEY_S) && reverseState != -1));

    reverseIsPressedOneTime = (getKeyState(KEY_W) || getKeyState(KEY_S));

    if (reverseState == 0)
        return;

    if (fieldWeakShaft == 0)
        mainShaftHeight = 0.0;

    // Авт. сброс
    if (getKeyState(KEY_D))
    {
        if (isControl())
        {
            if (autoReset)
                emit soundPlay("21KR_-A_0");
            autoReset = false;
        }
        if (!autoReset && isShift() && fieldWeakShaft != 0 && lastControllerPositionIsZero && is_dec)
        {
            fieldWeakShaft -= 2;
            emit soundPlay("21KR_op-");
            is_dec = false;
        } else {
            if(is_dec && fieldWeakShaft == 0 && !isControl())
                emit soundPlay("21KR_0_-");
            is_dec = false;
        }
    }
    else
    {
        if (!is_dec && fieldWeakShaft == 0)
            emit soundPlay("21KR_-_0");
        //if (TO_INT(fieldWeakShaft) == 0)
        is_dec = true;
    }

    if (autoReset)
    {
        autoReset = !getKeyState(KEY_A);
        return;
    }

    // 1 вниз
    if (getKeyState(KEY_A))
    {
        if(isShift() && fieldWeakShaft != 10 && lastControllerPositionIsZero && is_inc)
        {
            if (fieldWeakShaft == 0)
                mainShaftHeight = 1.0;

            if (getY(0) > 0.99)
            {
                fieldWeakShaft += 2;
                emit soundPlay("21KR_op+");
                is_inc = false;
            }

            lastControllerPositionIsZero = false;
        } else {
            if (is_inc && fieldWeakShaft == 0)
                emit soundPlay("21KR_0_+");
            is_inc = false;
        }
    }
    else
    {
        if (!is_inc && fieldWeakShaft == 0)
            emit soundPlay("21KR_+_0");
        is_inc = true;
    }

    // Авт. набор
    if (getKeyState(KEY_Q))
    {
        if (!autoSet)
            emit soundPlay("21KR_0_+A");
        autoSet = true;
    }
    else
    {
        if (autoSet)
            emit soundPlay("21KR_+A_0");
        autoSet = false;
    }

    // 1 вверх
    if (getKeyState(KEY_E))
    {
        if (!autoReset)
            emit soundPlay("21KR_0_-A");
        autoReset = true;
    }

    mainShaftPos = (-10 * autoReset) + (4 * autoSet) +
                   (!autoReset && !autoSet && !isShift() && !isControl()) *
                   (-5 * getKeyState(KEY_D) +
                     2 * getKeyState(KEY_A));

    mainShaftPos = mainShaftPos * TO_INT(hs_n(mainShaftHeight - 0.99));

    lastControllerPositionIsZero = (TO_INT(mainShaftPos) == 0);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::stepExternalControl(double t, double dt)
{
    Q_UNUSED(t)
    Q_UNUSED(dt)

    connectSignals(KM_K01, k01);
    connectSignals(KM_K02, k02);
    connectSignals(KM_K21, k21);
    connectSignals(KM_K22, k22);
    connectSignals(KM_K23, k23);
    connectSignals(KM_K25, k25);
    connectSignals(KM_K31, k31);
    connectSignals(KM_K32, k32);
    connectSignals(KM_K33, k33);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::connectSignals(ControllerSignals cs, bool &k)
{
    if (control_signals.analogSignal[cs].is_active)
        k = static_cast<bool>(control_signals.analogSignal[cs].cur_value);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::addSignalsInControllerState()
{
    controlState.k01 = k01;
    controlState.k02 = k02;

    controlState.k21 = k21;
    controlState.k22 = k22;
    controlState.k23 = k23;
    controlState.k25 = k25;

    controlState.k31 = k31;
    controlState.k32 = k32;
    controlState.k33 = k33;
}
