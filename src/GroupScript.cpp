
#include "ScriptMgr.h"
#include "MythicPlus.h"

// this handles updating custom group difficulties used in auto balancing mobs and
// scripts that enable buffs on mobs randomly
class MythicPlus_GroupScript : public GroupScript
{
    public:
    MythicPlus_GroupScript() : GroupScript("MythicPlus_GroupScript") { }

    void OnCreate(Group* group, Player* leader) override {
        if (!group) {
            return;
        }

        if(!leader) {
            return;
        }

    }

    void OnDisband(Group* group) override {
        if (!group) {
            return;
        }

    }
};

void AddGroupScripts()
{
    new MythicPlus_GroupScript();
}