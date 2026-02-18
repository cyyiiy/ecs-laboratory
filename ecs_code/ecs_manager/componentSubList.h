#pragma once
#include <bitset>


/**
 * Sublist of components, where the components objects are located in memory.
 * Each sublist have a fixed and compile-time number of components, defined by the component class.
 * Components are packed together in the sublist for a fast and cache-friendly iteration.
 * 
 * Note: Sublists are entirely managed by `ComponentManager` objects. It is advised to never access sublists directly.
 * 
 * @tparam T The component class.
 * @tparam SublistSize The number of components that can be contained in the sublist.
 */
template <class T, size_t SublistSize>
class ComponentSubList
{
    static_assert(std::is_base_of_v<class Component, T>, "T must be derived from Component.");
    
public:
    static constexpr uint32_t INVALID_INDEX = std::numeric_limits<uint32_t>::max();
    
    // The components memory, reserved at sublist creation
    std::aligned_storage_t<sizeof(T), alignof(T)> packedComponents[SublistSize];
    
    uint32_t slotToPacked[SublistSize]{ INVALID_INDEX };
    uint32_t packedToSlot[SublistSize]{ INVALID_INDEX };
    uint32_t generations[SublistSize]{};
    
    std::bitset<SublistSize> usedSlots{};
    
    size_t aliveCount{ 0 };
    size_t freeSlots{ SublistSize };

    /** Get a pointer to a component in the sublist.
     * 
     * @param packedIndex The index in the sublist aligned storage.
     * @return A pointer to the component.
     */
    T* packedGet(size_t packedIndex)
    {
        return std::launder(
            reinterpret_cast<T*>(&packedComponents[packedIndex])
        );
    }
};