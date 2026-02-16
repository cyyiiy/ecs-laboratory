#pragma once
#include <bitset>


template <class T, size_t SublistSize>
class ComponentSubList
{
public:
    static constexpr uint32_t INVALID_INDEX = std::numeric_limits<uint32_t>::max();
    
    std::aligned_storage_t<sizeof(T), alignof(T)> packedComponents[SublistSize];
    
    uint32_t slotToPacked[SublistSize]{ INVALID_INDEX };
    uint32_t packedToSlot[SublistSize]{ INVALID_INDEX };
    uint32_t generations[SublistSize]{};
    
    std::bitset<SublistSize> usedSlots{};
    
    size_t aliveCount{ 0 };
    size_t freeSlots{ SublistSize };
    
    T* packedGet(size_t packedIndex)
    {
        return std::launder(
            reinterpret_cast<T*>(&packedComponents[packedIndex])
        );
    }
};