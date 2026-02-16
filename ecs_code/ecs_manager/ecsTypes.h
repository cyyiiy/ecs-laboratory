#pragma once

using ComponentTypeId = size_t;

template <class T>
ComponentTypeId GetComponentTypeId()
{
    // 'id' serve as a static address unique for each Component type
    // This function returns a unique and stable value for each type it's invoked with
    static const char id = 0;
    return reinterpret_cast<ComponentTypeId>(&id);
}

struct RawComponentHandle
{
    uint32_t sublistId;
    uint32_t slotId;
    uint32_t generation;
};

template <class T>
struct ComponentHandle
{
    static_assert(std::is_base_of_v<class Component, T>, "T must be derived from Component.");
    
    RawComponentHandle raw;
    
    ComponentHandle(RawComponentHandle r) : raw(r) {}
};