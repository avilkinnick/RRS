#include    "chs2t-brake-mech.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS2tBrakeMech::CHS2tBrakeMech(QObject *parent)
    : BrakeMech(parent)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS2tBrakeMech::~CHS2tBrakeMech()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2tBrakeMech::load_config(CfgReader &cfg)
{
    BrakeMech::load_config(cfg);
}
