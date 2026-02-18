#include "entity.h"
#include <iostream>

void Entity::clearAllComponents()
{
    // 'pair' is: std::pair<ComponentTypeId, std::vector<StoredComponent>>
    for (auto& pair : components)
    {
        // Loop through all components of a class
        for (StoredComponent& stored_component : pair.second)
        {
            stored_component.deleteFunction(stored_component.raw_handle);
        }
    }
    
    components.clear();
}

void Entity::debugEntity()
{
    std::cout << "========= DEBUG Entity ========\n";
    for (auto& pair : components)
    {
        std::cout << " --- ComponentTypeId: " << pair.first << "\n";
        for (StoredComponent& stored_component : pair.second)
        {
            const RawComponentHandle& handle = stored_component.raw_handle;
            std::cout << "ComponentHandle: Sublist " << handle.sublistId << " | Slot " << handle.slotId << " | Generation " << handle.generation << "\n";
        }
    }
    std::cout << "===============================\n";
}
