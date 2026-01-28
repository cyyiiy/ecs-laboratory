#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include "component.h"
#include "componentSubList.h"


class IComponentManager
{
public:
    virtual ~IComponentManager() = default;
    
    virtual void UpdateComponents(float deltaTime) = 0;
    virtual void DeletePendingComponents() = 0;
    virtual void ClearAllComponents() = 0;
    
    virtual std::string DebugComponentManager() const = 0;
};


template <class T, size_t SublistSize>
class ComponentManager final : public IComponentManager
{
private:
    using Sublist = ComponentSubList<T, SublistSize>;
    std::vector<std::unique_ptr<Sublist>> sublists;
    std::vector<ComponentHandle> pendingComponents;
    
public:
    ComponentHandle CreateComponent()
    {
        // Step 1: Find a sublist with a free component slot
        size_t sublistId = sublists.size();
        for (size_t i = 0; i < sublists.size(); i++)
        {
            if (sublists[i]->freeSlots <= 0) continue;
            
            sublistId = i;
            break;
        }
        
        // Step 1 bis: Create a new sublist (if all existing ones are full)
        if (sublistId == sublists.size())
        {
            sublists.emplace_back(std::make_unique<Sublist>());
        }
        
        Sublist& sublist_creating_in = *sublists[sublistId];
        
        // Step 2: Find a free component slot in the sublist
        size_t slotId = SublistSize;
        for (size_t i = 0; i < SublistSize; i++)
        {
            if (sublist_creating_in.usedSlots[i]) continue;
            
            slotId = i;
            break;
        }
        
        if (slotId == SublistSize)
        {
            throw std::runtime_error("Failed to find a free component slot.");
        }
        
        sublist_creating_in.usedSlots[slotId] = true;
        --sublist_creating_in.freeSlots;
        
        // Step 3: Construct the component
        new (sublist_creating_in.get(slotId)) T();
        sublist_creating_in.get(slotId)->init();
        
        // Step 4: Return the component handle
        const uint32_t generation = sublist_creating_in.generations[slotId];
        return ComponentHandle{ 
            static_cast<std::uint32_t>(sublistId), 
            static_cast<std::uint32_t>(slotId),
            generation
        };
    }
    
    void DeleteComponent(const ComponentHandle& handle)
    {
        T& component = GetComponent(handle);
        
        if (component.getPendingDelete())
            throw std::runtime_error("Component is already pending deletion.");
            
        component.setPendingDelete(true);
        
        pendingComponents.push_back(handle);
    }
    
    T& GetComponent(const ComponentHandle& handle)
    {        
        Sublist& sublist = *sublists[handle.sublistId];
        
        if (sublist.generations[handle.slotId] != handle.generation)
            throw std::runtime_error("Invalid component handle.");
        
        return *sublists[handle.sublistId]->get(handle.slotId);
    }
    
    // Note: Called automatically every frame by the system that manages the ECS globally
    void DeletePendingComponents() override
    {
        if (pendingComponents.empty()) return;
        
        for (auto& handle : pendingComponents)
        {
            Sublist& sublist = *sublists[handle.sublistId];
        
            if (sublist.generations[handle.slotId] != handle.generation)
                throw std::runtime_error("Invalid component handle.");
            
            T* component = sublist.get(handle.slotId);
            component->exit();
        
            component->~T();
            ++sublist.generations[handle.slotId];
        
            sublist.usedSlots[handle.slotId] = false;
            ++sublist.freeSlots;
        }
        
        pendingComponents.clear();
    }
    
    // Note: Called automatically every frame by the system that manages the ECS globally
    void UpdateComponents(float deltaTime) override
    {
        for (size_t i = 0; i < sublists.size(); i++)
        {
            Sublist& sublist = *sublists[i];
            
            for (size_t j = 0; j < SublistSize; j++)
            {
                if (!sublist.usedSlots[j]) continue;
                
                T* component = sublist.get(j);
                if (!component->getUpdateActivated()) continue;
                
                component->update(deltaTime);
            }
        }
    }
    
    // Note: Could be called by the system that manages the ECS globally when closing the game for example
    void ClearAllComponents() override
    {
        for (auto& sublist_ptr : sublists)
        {
            Sublist& sublist = *sublist_ptr;
            
            for (size_t i = 0; i < SublistSize; i++)
            {
                if (!sublist.usedSlots[i]) continue;
                
                T* component = sublist.get(i);
                component->exit();
                component->~T();
                
                sublist.usedSlots[i] = false;
                ++sublist.generations[i];
            }
            
            sublist.freeSlots = SublistSize;
        }
        
        pendingComponents.clear();
    }
    
    
    std::string DebugComponentManager() const override
    {
        std::stringstream result;
        result << "Manager for component \"" << typeid(T).name() << "\"\n";
        
        result << "Number of sublists: " << sublists.size() << "\n";
        result << "Number of components per sublist: " << SublistSize << "\n";
        
        for (size_t i = 0; i < sublists.size(); i++)
        {
            result << "Free slots of sublist " << i << ": " << sublists[i]->freeSlots << "\n";
            for (size_t j = 0; j < SublistSize; j++)
            {
                if (sublists[i]->usedSlots[j] || sublists[i]->generations[j] != 0)
                {
                    result << "Sublist " << i << " | Slot " << j << " | Used: " << (sublists[i]->usedSlots[j] ? "true" : "false") << " | Generation: " << sublists[i]->generations[j] << "\n";
                }
            }
        }
        
        return result.str();
    }
};