
#include "Log.h"

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
    LOG_DEBUG("module.MythicPlus", "[MythicPlus] Initializing scripts.");
    Add_MP_AllCreatureScripts();
    Add_MP_AllMapScripts();
    Add_MP_CommandScripts();
    Add_MP_GlobalScripts();
    Add_MP_GroupScripts();
    Add_MP_PlayerScripts();
    Add_MP_UnitScripts();
    Add_MP_WorldScripts();
}
