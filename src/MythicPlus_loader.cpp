#include "MpScheduler.h"
#include "MpLogger.h"

// Creature Overrides
enum {
    RAGEFIRE_BAZZALAN       = 11519
};

// This adds schedulers for use across scripts scoped to MythicPlus
void Add_MP_Schedulers()
{
    MpLogger::debug("Add_MP_Schedulers()");
    new MpScheduler_WorldScript();
    sMpScheduler->StartScheduler();
}

void Addmod_mythic_plusScripts();
void Add_MP_AllCreatureScripts();
void Add_MP_AllMapScripts();
void Add_MP_CommandScripts();
void Add_MP_GlobalScripts();
void Add_MP_GroupScripts();
void Add_MP_PlayerScripts();
void Add_MP_UnitScripts();
void Add_MP_WorldScripts();

void Addmod_mythic_plusScripts()
{
    Add_MP_AllCreatureScripts();
    Add_MP_AllMapScripts();
    Add_MP_CommandScripts();
    Add_MP_GlobalScripts();
    // Add_MP_GroupScripts();
    Add_MP_PlayerScripts();
    Add_MP_UnitScripts();
    Add_MP_WorldScripts();
    Add_MP_Schedulers();

    // new Ragefire_Bazzalan_Mythic();

    // list of boss / creature event handlers
    // new Ragefire_Bazzalan_Mythic(RAGEFIRE_BAZZALAN);

}
