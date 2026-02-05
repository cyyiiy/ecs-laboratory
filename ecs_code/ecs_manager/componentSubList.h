#pragma once
#include <bitset>


template <class T, size_t SublistSize>
class ComponentSubList
{
public:
    std::aligned_storage_t<sizeof(T), alignof(T)> components[SublistSize];
    std::bitset<SublistSize> usedSlots{};
    uint32_t generations[SublistSize]{};
    size_t freeSlots{ SublistSize };
    
    T* get(size_t index)
    {
        return std::launder(reinterpret_cast<T*>(&components[index]));
    }
};