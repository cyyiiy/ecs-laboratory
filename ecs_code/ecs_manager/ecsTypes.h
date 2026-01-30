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


struct ComponentHandle
{
    uint32_t sublistId;
    uint32_t slotId;
    uint32_t generation;
    
    bool operator==(const ComponentHandle& other) const
    {
        return 
            sublistId == other.sublistId &&
            slotId == other.slotId &&
            generation == other.generation;
    }
};