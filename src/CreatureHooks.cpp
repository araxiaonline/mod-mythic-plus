#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Creature.h"


class CreatureHooks {
private:
    // Define type aliases for cleaner code
    using EventCallback = std::function<void(Creature*)>;
    using EventList = std::vector<EventCallback>;
    using EventMap = std::unordered_map<uint32, EventList>;

    // Constructor
    CreatureHooks() = default;

    // Use the type aliases for readability
    std::unique_ptr<EventMap> _spawnedEvents = std::make_unique<EventMap>();
    std::unique_ptr<EventMap> _isDeadEvents = std::make_unique<EventMap>();
    std::unique_ptr<std::unordered_map<uint32, std::unordered_map<uint8, EventList>>> _healthPercentEvents
        = std::make_unique<std::unordered_map<uint32, std::unordered_map<uint8, EventList>>>();

public:
    // Singleton instance access method
    static CreatureHooks* instance() {
        static CreatureHooks instance;
        return &instance;
    }

    // Register events for health at a certain percentage
    void RegisterHealthAtPercentHook(uint32 entry, uint8 percent, EventCallback callback) {
        (*_healthPercentEvents)[entry][percent].push_back(callback);
    }

    // Register "Is Dead" events
    void RegisterIsDeadHook(uint32 entry, EventCallback callback) {
        (*_isDeadEvents)[entry].push_back(callback);
    }

    // Register "Spawned" events
    void RegisterSpawnedHook(uint32 entry, EventCallback callback) {
        (*_spawnedEvents)[entry].push_back(callback);
    }

    // Call health events if the creature's health is at or below the percentage
    void CheckHealthEvents(Creature* creature) {
        uint32 entry = creature->GetEntry();
        uint32 currentHealthPct = creature->GetHealthPct();

        if (_healthPercentEvents->contains(entry)) {
            for (const auto& [percent, callbacks] : _healthPercentEvents->at(entry)) {
                if (currentHealthPct <= percent) {
                    for (auto& callback : callbacks) {
                        callback(creature); // Trigger custom behavior
                    }
                }
            }
        }
    }

    // Call "Is Dead" events if the creature is dead
    void CheckIsDeadEvent(Creature* creature) {
        uint32 entry = creature->GetEntry();
        if (!creature->IsAlive() && _isDeadEvents->contains(entry)) {
            for (auto& callback : _isDeadEvents->at(entry)) {
                callback(creature); // Trigger custom behavior
            }
        }
    }

    // Call "Spawned" events when the creature spawns
    void CheckSpawnedEvent(Creature* creature) {
        uint32 entry = creature->GetEntry();
        if (_spawnedEvents->contains(entry)) {
            for (auto& callback : _spawnedEvents->at(entry)) {
                callback(creature); // Trigger custom behavior
            }
        }
    }
};
