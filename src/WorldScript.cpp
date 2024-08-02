#include "ScriptMgr.h"
#include "MythicPlus.h"
#include "Config.h"
#include "Player.h"

class MythicPlus_WorldScript : public WorldScript
{
public:
    MythicPlus_WorldScript() : WorldScript("MythicPlus_WorldScript") { }

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        SetInitialWorldSettings();
        // sMythicPlus->lastConfigTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void OnStartup() override
    {

    }

    void SetInitialWorldSettings()
    {
        // sMythicPlus->configValue = sConfigMgr->GetOption<float>("MythicPlus.ConfigValue", 1.0f, false);

    }
};

void AddWorldScripts()
{
    new MythicPlus_WorldScript();
}
