#include "CreatureHooks.h"
#include "MpLogger.h"

class BaseCreatureHandler {
public:
    BaseCreatureHandler(uint32 entry) {

        ASSERT(entry > 0);

        MpLogger::debug("Registering JustDied and OnSpawn events for entry: ", entry);

        // Register the JustDied event
        sCreatureHooks->RegisterJustDied(entry, [this](Creature* creature, Unit* killer) {
            this->OnJustDied(creature, killer);
        });

        // Register the OnSpawn event
        sCreatureHooks->RegisterOnSpawn(entry, [this](Creature* creature) {
            this->OnJustSpawned(creature);
        });

        sCreatureHooks->RegisterOnAddToInstance(entry, [this](Creature* creature) {
            this->OnAddToInstance(creature);
        });
    }

    // Virtual event handlers to be overridden by bosses
    virtual void OnJustDied(Creature* /*creature*/, Unit* /*killer*/) {}

    virtual void OnJustSpawned(Creature* /*creature*/) {}

    virtual void OnAddToInstance(Creature* /*creature*/) {}

    virtual ~BaseCreatureHandler() {}
};
