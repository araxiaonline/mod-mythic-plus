
#include "MpDataStore.h"
#include "MpLogger.h"
#include "ScriptMgr.h"
#include "Group.h"

// this handles updating custom group difficulties used in auto balancing mobs and
// scripts that enable buffs on mobs randomly
class MythicPlus_GroupScript : public GroupScript
{
    public:
    MythicPlus_GroupScript() : GroupScript("MythicPlus_GroupScript") { }

    void OnAddMember(Group* group, ObjectGuid guid) override {
        if (!group || !guid) {
            return;
        }

        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player) {
            MpLogger::warn("Player not found for guid {}", guid.GetCounter());
            return;
        }

        MpGroupData *gd = sMpDataStore->GetGroupData(group->GetGUID());
        MpPlayerData* pd = sMpDataStore->GetPlayerData(guid);
        if(!pd) {

            MpDifficulty difficulty = GetPlayerDifficulty(player);
            MpPlayerData playerData = MpPlayerData(player, difficulty, group->GetGUID().GetCounter());
            sMpDataStore->AddPlayerData(guid, &playerData);
        } else {

            // If the player is joining a new group then reset the death counters otherwise let them ride
            if (pd->groupId != group->GetGUID().GetCounter()) {
                pd->groupId = group->GetGUID().GetCounter();
                pd->ResetAllDeathCounts();
            }
        }

        if(!gd) {
            MpLogger::warn("Group data not found for group {}", group->GetGUID().GetCounter());
            return;
        }

        gd->AddPlayerData(pd);
    }

    void OnCreate(Group* group, Player* leader) override {
        if (!group) {
            return;
        }

        if(!leader) {
            return;
        }

        // Start a group and set the data up for the group
        MpDifficulty difficulty = GetPlayerDifficulty(leader);
        MpGroupData gd = MpGroupData(group, difficulty);

        // Insert the leader of the group after group data struct is built
        MpPlayerData* pd = sMpDataStore->GetPlayerData(leader->GetGUID());
        if(pd) {
            gd.AddPlayerData(pd);
        } else {
            MpPlayerData playerData = MpPlayerData(leader, difficulty, group->GetGUID().GetCounter());
            gd.AddPlayerData(&playerData);
        }

        // Store the data into our memory store
        sMpDataStore->AddGroupData(group, gd);
    }

    void OnDisband(Group* group) override {
        sMpDataStore->RemoveGroupData(group);
    }

    // Get the difficulty for a player that is assigned
    MpDifficulty GetPlayerDifficulty(Player* player) {
        if(!player) {
            return MP_DIFFICULTY_NORMAL;
        }

        MpPlayerData* pd = sMpDataStore->GetPlayerData(player->GetGUID());
        if(pd) {
            return pd->difficulty;
        } else {
            return player->GetDifficulty(false) == Difficulty::DUNGEON_DIFFICULTY_NORMAL ? MP_DIFFICULTY_NORMAL : MP_DIFFICULTY_HEROIC;
        }

        return MP_DIFFICULTY_NORMAL;
    }
};

void Add_MP_GroupScripts()
{
    MpLogger::debug("Add_MP_GroupScripts()");
    new MythicPlus_GroupScript();
}
