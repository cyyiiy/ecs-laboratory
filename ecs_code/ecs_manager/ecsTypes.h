#pragma once
#include <cstdint>
#include <type_traits>

using ComponentTypeId = size_t;

/** Get a unique and stable identifier for a component class.
 * 
 * @tparam T The component class.
 * @return The unique identifier.
 */
template <class T>
ComponentTypeId GetComponentTypeId()
{
    // 'id' serve as a static address unique for each Component type
    // This function returns a unique and stable value for each type it's invoked with
    static const char id = 0;
    return reinterpret_cast<ComponentTypeId>(&id);
}

/**
 * Non-templated handle that allows to retrieve a component from a manager.
 * 
 * Note: This exists for internal reasons, prefer `ComponentHandle` for basic ECS usage.
 */
struct RawComponentHandle
{
    uint32_t sublistId;
    uint32_t slotId;
    uint32_t generation;
};

/**
 * Templated handle that allows to retrieve a component from a manager.
 * 
 * @tparam T The component class.
 */
template <class T>
struct ComponentHandle
{
    static_assert(std::is_base_of_v<class Component, T>, "T must be derived from Component.");
    
    RawComponentHandle raw;
    
    ComponentHandle(RawComponentHandle r) : raw(r) {}
};