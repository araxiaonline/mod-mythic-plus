#ifndef CREATUREHOOKS_H
#define CREATUREHOOKS_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Creature.h"
#include "ObjectGuid.h"

// Struct to store the state of which handlers have been fired for a creature
struct CreatureEventState {
    bool onDeathEventFired = false;
    bool onSpawnEventFired = false;
    bool onAddedToInstanceEventFired = false;
    uint8 deaths = 0;  // Count of deaths
};

// Type aliases for creature event handling
// using CreatureHook = std::function<void(Creature*)>;
// using HookList = std::vector<CreatureHook>;
// using HandlerMap = std::unordered_map<uint32, HookList>;

// Type alias for variadic event hooks
template<typename... Args>
using CreatureHook = std::function<void(Args...)>;

// Type alias for a list of hooks that take varying arguments
template<typename... Args>
using HandlersList = std::vector<CreatureHook<Args...>>;

// HandlerMap using variadic templates
template<typename... Args>
using HandlerMap = std::unordered_map<uint32, HandlersList<Args...>>;

using CreatureEventStateMap = std::map<ObjectGuid, CreatureEventState>;

class CreatureHooks {
private:

    CreatureHooks() { }
    ~CreatureHooks() { }

    // ensure we only ever have one instance of this class
    CreatureHooks(const CreatureHooks&) = delete;
    CreatureHooks& operator=(const CreatureHooks&) = delete;

    // Data members for storing event handlers
    std::unique_ptr<HandlerMap<Creature*>> _OnSpawnHandlers;
    std::unique_ptr<HandlerMap<Creature*, Unit*>> _JustDiedHandlers;
    std::unique_ptr<HandlerMap<Creature*>> _OnAddToInstanceHandlers;

    // Tracks state to know which handlers need to be fired again
    std::unique_ptr<CreatureEventStateMap> _eventStates;

public:
    static CreatureHooks* instance() {
        static CreatureHooks instance;
        return &instance;
    }

    // Register events for specific actions
    void RegisterJustDied(uint32 entry, CreatureHook<Creature*, Unit*> callback);
    void RegisterOnSpawn(uint32 entry, CreatureHook<Creature*> callback);
    void RegisterOnAddToInstance(uint32 entry, uint32 mapId, uint32 instanceId, CreatureHook<Creature*> callback);

    // Event triggers
    void JustDied(Creature* creature, Unit* killer);
    void JustSpawned(Creature* creature);
};

#define sCreatureHooks CreatureHooks::instance()

#endif // CREATUREHOOKS_H
