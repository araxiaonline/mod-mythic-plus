#include "ScriptMgr.h"
#include "MythicPlus.h"
#include "Player.h"

class MythicPlus_UnitScript : public UnitScript
{
public:
    MythicPlus_UnitScript() : UnitScript("MythicPlus_UnitScript", true) { }


    void OnAuraApply(Unit* unit, Aura* aura) override {

    }

};

void Add_MP_UnitScripts()
{
    static MythicPlus* mp = MythicPlus::getInstance();
    mp->debug("Add_MP_UnitScripts()");
    new MythicPlus_UnitScript();
}
