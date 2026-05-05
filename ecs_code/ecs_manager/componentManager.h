#pragma once
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include "componentSubList.h"
#include "ecsTypes.h"


/**
 * Interface for `ComponentManager` so a list of all managers is possible (for global ECS methods such as `DeletePendings`).
 */
class IComponentManager
{
public:
    virtual ~IComponentManager() = default;
    
    virtual void DeletePendingComponents() = 0;
    virtual void ClearAllComponents(bool closeEngine) = 0;
    
    virtual std::string DebugComponentManager() const = 0;
};

/**
 * ECS manager that is responsible for all components of a class.
 * The components are stored in `ComponentSubList` objects, and the system works like a memory pool.
 * 
 * Note: Component managers are created at compilation by the ECS system. It is advised to never create one manually.
 * 
 * @tparam T The component class.
 * @tparam SublistSize The number of components that can be contained in a single sublist for this class.
 */
template <class T, size_t SublistSize>
class ComponentManager : public IComponentManager
{
    static_assert(std::is_base_of_v<Component, T>, "T must be derived from Component.");
    static_assert(std::is_move_constructible_v<T>, "T must be move constructible.");
    
protected:
    static constexpr uint32_t INVALID_INDEX = std::numeric_limits<uint32_t>::max();
    
    using Sublist = ComponentSubList<T, SublistSize>;
    std::vector<std::unique_ptr<Sublist>> sublists;
    std::vector<RawComponentHandle> pendingComponents;
    
public:
    /** Creates a new component in the manager.
     * 
     * Note: Prefer creating a component from an entity for a basic usage of the ECS.
     * 
     * @param ownerEntity The entity that owns this component. Must be nullptr if this function is not called from `Entity::addComponentByClass`.
     * @return A ComponentHandle that allows to retrieve and access the component.
     */
    ComponentHandle<T> CreateComponent(class Entity* ownerEntity)
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
        
        // Step 3: Set the packed index of the new component in the sublist
        uint32_t packedIndex = static_cast<uint32_t>(sublist_creating_in.aliveCount++);
        
        sublist_creating_in.slotToPacked[slotId] = packedIndex;
        sublist_creating_in.packedToSlot[packedIndex] = static_cast<uint32_t>(slotId);

        const uint32_t generation = sublist_creating_in.generations[slotId];
        const RawComponentHandle raw_handle{
            static_cast<uint32_t>(sublistId),
            static_cast<uint32_t>(slotId),
            generation
        };
        
        // Step 4: Construct the component
        T* component = sublist_creating_in.packedGet(packedIndex);
        
        new (component) T();
        component->setOwner(ownerEntity);
        component->setRawHandle(raw_handle);
        
        if constexpr (std::is_base_of_v<BehaviorComponent, T>)
        {
            component->init();
        }
        
        // Step 5: Return the component handle
        return ComponentHandle<T>(raw_handle);
    }

    /** Deletes an existing component from the manager.
     * 
     * Note: The deletion only occurs when `ComponentManager::DeletePendingComponents` is called.
     * 
     * @param handle The ComponentHandle that allows to access the component to delete.
     */
    void DeleteComponent(const ComponentHandle<T>& handle)
    {
        T& component = GetComponent(handle); // Throw error if the component doesn't exist
        
        if (component.getPendingDelete())
            throw std::runtime_error("Component is already pending deletion.");
            
        component.setPendingDelete(true);
        
        pendingComponents.push_back(handle.raw);
    }

    /** Get a reference to an existing component in the manager.
     * 
     * @param handle The ComponentHandle that allows to access the component to get.
     * @return A reference to the component.
     */
    T& GetComponent(const ComponentHandle<T>& handle)
    {        
        Sublist& sublist = *sublists[handle.raw.sublistId];
        
        if (sublist.generations[handle.raw.slotId] != handle.raw.generation)
            throw std::runtime_error("Invalid component handle.");
        
        // `slotToPacked` allows to retrieve the packed index of the component in the sublist from the slot index in the handle
        const uint32_t packed_index = sublist.slotToPacked[handle.raw.slotId];
        
        return *sublist.packedGet(packed_index);
    }

    /** Know if a component handle access to an existing component in the manager.
     * 
     * @param handle The ComponentHandle to check.
     * @return True if the component handle is valid, False otherwise.
     */
    bool IsComponentHandleValid(const ComponentHandle<T>& handle) noexcept
    {
        if (handle.raw.sublistId >= sublists.size() || handle.raw.slotId >= SublistSize)
        {
            return false;
        }
        
        Sublist& sublist = *sublists[handle.raw.sublistId];
        
        if (!sublist.usedSlots[handle.raw.slotId])
        {
            return false;
        }
        
        return sublist.generations[handle.raw.slotId] == handle.raw.generation;
    }

    /** Destroy all components in the manager that are pending deletion.
     * 
     * Note: Usually called once a frame by `ECS::DeletePendings`.
     */
    void DeletePendingComponents() override
    {
        if (pendingComponents.empty()) return;
        
        for (RawComponentHandle& handle : pendingComponents)
        {
            // Step 1: Prepare the data
            Sublist& sublist = *sublists[handle.sublistId];
        
            if (sublist.generations[handle.slotId] != handle.generation)
                continue;
            
            const uint32_t packed_index = sublist.slotToPacked[handle.slotId]; // packed index of the pending deletion component
            const uint32_t last_packed_index = static_cast<uint32_t>(sublist.aliveCount - 1); // packed index of the last alive component in the sublist
            
            T* component = sublist.packedGet(packed_index);
            
            // Step 2: Delete the pending deletion component
            if constexpr (std::is_base_of_v<BehaviorComponent, T>)
            {
                component->exit();
            }
            component->~T();
            
            if (packed_index != last_packed_index)
            {
                // Step 3: Move the last alive component in the sublist to keep memory consistency
                T* last_component = sublist.packedGet(last_packed_index);
                
                // Constructs a new T object at the memory address of the deleted component by using the move constructor
                new (component) T(std::move(*last_component));
                last_component->~T();
                
                // Step 4: Update the indirection table so the slot of the swapped component redirects to the new packed index
                uint32_t swaped_comp_slot = sublist.packedToSlot[last_packed_index];
                sublist.slotToPacked[swaped_comp_slot] = packed_index;
                sublist.packedToSlot[packed_index] = swaped_comp_slot;
            }
            
            // Step 4-bis: Properly resets the indirection table at indices of the deleted component
            sublist.slotToPacked[handle.slotId] = INVALID_INDEX;
            sublist.packedToSlot[last_packed_index] = INVALID_INDEX;
            
            // Step 5: Update sublist metadata
            --sublist.aliveCount;
            ++sublist.freeSlots;
            
            ++sublist.generations[handle.slotId];
            sublist.usedSlots[handle.slotId] = false;
        }
        
        pendingComponents.clear();
    }

    /** Iterates on all active components of the manager.
     * 
     * Usage:
     * 
     * manager.ForEach([](const Component& component) { component.doSomething(); });
     * 
     * @param func The lambda to execute for each component.
     */
    template<typename Func>
    void ForEach(Func&& func)
    {
        for (auto& sublist_ptr : sublists)
        {
            Sublist& sublist = *sublist_ptr;
            
            for (uint32_t packed_index = 0; packed_index < sublist.aliveCount; packed_index++)
            {
                func(*sublist.packedGet(packed_index));
            }
        }
    }

    /** Instantly delete every existing components of the manager.
     * 
     * Note: Usually called by `ECS::Clear`.
     */
    void ClearAllComponents(bool closeEngine) override
    {
        for (auto& sublist_ptr : sublists)
        {
            Sublist& sublist = *sublist_ptr;
            
            // Clear only alive components
            for (uint32_t packed_index = 0; packed_index < sublist.aliveCount; packed_index++)
            {
                const uint32_t slot = sublist.packedToSlot[packed_index];
                T* component = sublist.packedGet(packed_index);
                
                if constexpr (std::is_base_of_v<BehaviorComponent, T>)
                {
                    if (!closeEngine)
                    {
                        component->exit();
                    }
                }
                component->~T();
                
                ++sublist.generations[slot];
                sublist.usedSlots[slot] = false;
                
                sublist.slotToPacked[slot] = INVALID_INDEX;
                sublist.packedToSlot[packed_index] = INVALID_INDEX;
            }
            
            sublist.aliveCount = 0;
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
                    result << "Sublist " << i << " | Slot " << j << " | Used: " << (sublists[i]->usedSlots[j] ? "true" : "false") << " | Generation: " << sublists[i]->generations[j] << " | PackedIndex: " << sublists[i]->slotToPacked[j] << "\n";
                }
            }
        }
        
        return result.str();
    }
};


/**
 * Interface for `BehaviorManager` so a list of all managers of behavior components is possible (for global `Update`).
 */
class IBehaviorManager
{
public:
    virtual void UpdateComponents(float deltaTime) = 0;
};

/**
 * Override of `ComponentManager` for component class that inherit from `BehaviorComponent`.
 * 
 * Note: Behavior managers are created at compilation by the ECS system. It is advised to never create one manually.
 * 
 * @tparam T The component class.
 * @tparam SublistSize The number of components that can be contained in a single sublist for this class.
 */
template <class T, size_t SublistSize>
class BehaviorManager final : public ComponentManager<T, SublistSize>, public IBehaviorManager
{
    static_assert(std::is_base_of_v<BehaviorComponent, T>, "T must be derived from BehaviorComponent.");
    
public:
    // Note: Called automatically every frame by the system that manages the ECS globally
    void UpdateComponents(float deltaTime) override
    {
        auto& sublists = this->sublists;
        for (auto& sublist_ptr : sublists)
        {
            ComponentSubList<T, SublistSize>& sublist = *sublist_ptr;
            
            // Update only alive components
            for (uint32_t packed_index = 0; packed_index < sublist.aliveCount; packed_index++)
            {
                T* component = sublist.packedGet(packed_index);
                if (!component->getUpdateActivated()) continue;
                
                component->update(deltaTime);
            }
        }
    }
};