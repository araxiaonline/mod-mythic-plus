#include "MpLogger.h"
#include "Player.h"
#include "ScriptMgr.h"

class MythicPlus_UnitScript : public UnitScript
{
public:
    MythicPlus_UnitScript() : UnitScript("MythicPlus_UnitScript", true) { }


    void OnAuraApply(Unit* unit, Aura* aura) override {

    }

};

void Add_MP_UnitScripts()
{
    MpLogger::debug("Add_MP_UnitScripts()");
    new MythicPlus_UnitScript();
}
