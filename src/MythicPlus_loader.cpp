// #include "ScriptMgr.h"
// #include "Instances/Ragefire/boss_bazzalan.cpp"

void Addmod_mythic_plusScripts();
void Add_MP_AllCreatureScripts();
void Add_MP_AllMapScripts();
void Add_MP_CommandScripts();
void Add_MP_GlobalScripts();
void Add_MP_GroupScripts();
void Add_MP_PlayerScripts();
void Add_MP_UnitScripts();
void Add_MP_WorldScripts();

// Mythic custom encounters for mythic+ dungeons
void AddSC_Ragefire_Bazzalan_Mythic();

void Addmod_mythic_plusScripts()
{
    Add_MP_AllCreatureScripts();
    Add_MP_AllMapScripts();
    Add_MP_CommandScripts();
    Add_MP_GlobalScripts();
    // Add_MP_GroupScripts();
    // Add_MP_PlayerScripts();
    Add_MP_UnitScripts();
    Add_MP_WorldScripts();

    // Boss custom changes for mythic+ dungeons
    AddSC_Ragefire_Bazzalan_Mythic();
}
