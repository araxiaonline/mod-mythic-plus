
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

        MpDataStore* mpds = MpDataStore::getInstance();
        GroupData gd = { group, MP_DIFFICULTY_NORMAL };
        mpds->AddGroupData(group->GetGUID(), gd);
    }

    void OnDisband(Group* group) override {
        if (!group) {
            return;
        }

        MpDataStore* mpds = MpDataStore::getInstance();
        mpds->RemoveGroupData(group->GetGUID());
    }
};

void Add_MP_GroupScripts()
{
    MpLogger::debug("Add_MP_GroupScripts()");
    new MythicPlus_GroupScript();
}