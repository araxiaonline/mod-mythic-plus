#include "MpScheduler.h"
#include "MpLogger.h"
#include "Spells/AdvancmentSpells.cpp"

// Creature Overrides
enum {
    RAGEFIRE_BAZZALAN       = 11519
};

// This adds schedulers for use across scripts scoped to MythicPlus
void Add_MP_Schedulers() {
    new MpScheduler_WorldScript();
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
void Add_MP_PlayerMessageEvents();

// Spell Scripts
void AddSC_AdvancementSpells();

void Addmod_mythic_plusScripts() {
    Add_MP_AllCreatureScripts();
    Add_MP_AllMapScripts();
    Add_MP_CommandScripts();
    Add_MP_GlobalScripts();
    Add_MP_PlayerScripts();
    Add_MP_UnitScripts();
    Add_MP_WorldScripts();
    Add_MP_Schedulers();
    Add_MP_PlayerMessageEvents();

    // Spell Scripts
    AddSC_AdvancementSpells();

    // new Ragefire_Bazzalan_Mythic();
    // Add_MP_GroupScripts();
    // list of boss / creature event handlers
    // new Ragefire_Bazzalan_Mythic(RAGEFIRE_BAZZALAN);

}
