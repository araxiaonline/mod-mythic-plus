
#include "MpDataStore.h"
#include "MpLogger.h"
#include "ScriptMgr.h"

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

        // sMpDataStore->AddGroupData(group->GetGUID(), gd);
    }

    void OnDisband(Group* group) override {
        sMpDataStore->RemoveGroupData(group);
    }
};

void Add_MP_GroupScripts()
{
    MpLogger::debug("Add_MP_GroupScripts()");
    new MythicPlus_GroupScript();
}
