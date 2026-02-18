#pragma once
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "ecsTypes.h"
#include "ecs.h"
#include "component.h"


/**
 * Internal struct that allows the entity to store a component handle with a pointer to the delete function templated for the correct class.
 * (Useful for the automatic deletion of every component on the entity.)
 */
struct StoredComponent
{
    RawComponentHandle raw_handle;
    void (*deleteFunction)(const RawComponentHandle&);
    
    // Custom == operator to allow std::find to work by searching a RawComponentHandle in a list of StoredComponent
    bool operator==(const RawComponentHandle& otherRaw) const
    {
        return 
            otherRaw.sublistId == this->raw_handle.sublistId && 
            otherRaw.slotId == this->raw_handle.slotId &&
            otherRaw.generation == this->raw_handle.generation;
    }
};


/**
 * Object that can own components and easily manage them.
 */
class Entity
{
public:
    Entity() = default;
    ~Entity() = default;
    
    Entity(const Entity& other) = delete;
    Entity& operator=(const Entity&) = delete;


    /** Creates a new component of the given class in the ECS and assign it to this entity.
     * 
     * @tparam T The component class.
     * @return A ComponentHandle that allows to retrieve and access the component.
     */
    template<class T>
    ComponentHandle<T> addComponentByClass()
    {
        const ComponentTypeId type_id = GetComponentTypeId<T>();
        const ComponentHandle<T> handle = ECS::CreateComponent<T>(this);
        
        components[type_id].push_back(
            StoredComponent{
                handle.raw,
                &ECS::DeleteComponentRawHandle<T>
            });
        
        return handle;
    }

    /** Know if a component handle access to a component owned by this entity.
     * 
     * @tparam T The component class. | This function can be called without explicitly using the template argument as it is deduced from the ComponentHandle.
     * @param handle The ComponentHandle to check.
     * @return True if the component exists on this entity, False otherwise.
     */
    template<class T>
    bool hasComponent(ComponentHandle<T> handle) const noexcept
    {
        const ComponentTypeId type_id = GetComponentTypeId<T>();
        
        if (components.find(type_id) == components.end())
            return false;

        const auto iter = 
            std::find(components.at(type_id).begin(), components.at(type_id).end(), handle.raw);
        
        return iter != components.at(type_id).end();
    }

    /** Get a reference to an existing component owned by this entity.
     * 
     * @tparam T The component class. | This function can be called without explicitly using the template argument as it is deduced from the ComponentHandle.
     * @param handle The ComponentHandle that allows to access the component to get.
     * @return A reference to the component.
     */
    template<class T>
    T& getComponent(ComponentHandle<T> handle)
    {
        if (!hasComponent(handle))
            throw std::runtime_error("Component doesn't belongs to this entity.");
        
        return ECS::GetComponent(handle);
    }

    /** Know if this entity owns at least one component of the given class.
     * 
     * @tparam T The component class.
     * @return True if this entity owns a component of the given class, False otherwise.
     */
    template<class T>
    bool hasComponentOfClass() const noexcept
    {
        const ComponentTypeId type_id = GetComponentTypeId<T>();
        
        return components.find(type_id) != components.end() && components.at(type_id).size() > 0;
    }

    /** Returns a component handle linked to the first component of the given class owned by this entity.
     * 
     * @tparam T The component class.
     * @return A ComponentHandle linked to the first component of the given class owned by this entity.
     */
    template<class T>
    ComponentHandle<T> getComponentOfClass()
    {
        if (!hasComponentOfClass<T>())
            throw std::runtime_error("Entity doesn't have a component of this class.");
        
        const ComponentTypeId type_id = GetComponentTypeId<T>();
        const ComponentHandle<T> handle(components[type_id][0].raw_handle);
        
        return handle;
    }

    /** Returns a list of every component (as component handles) of the given class owned by this entity.
     * 
     * @tparam T The component class.
     * @return A list of ComponentHandle linked to every component of the given class owned by this entity.
     */
    template<class T>
    std::vector<ComponentHandle<T>> getAllComponentsOfClass()
    {
        const ComponentTypeId type_id = GetComponentTypeId<T>();
        
        if (components.find(type_id) == components.end())
            throw std::runtime_error("Entity doesn't have a component of this class.");
        
        if (components[type_id].size() == 0)
            throw std::runtime_error("Entity doesn't have a component of this class.");
        
        std::vector<ComponentHandle<T>> return_list;
        return_list.reserve(components[type_id].size());
        
        for (StoredComponent stored_handle : components[type_id])
        {
            // Construct templated handles from the raw stored ones
            return_list.push_back(ComponentHandle<T>(stored_handle.raw_handle));
        }
        
        return return_list;
    }

    /** Remove an existing component owned by this entity.
     * 
     * @tparam T The component class. | This function can be called without explicitly using the template argument as it is deduced from the ComponentHandle.
     * @param handle The ComponentHandle that allows to access the component to delete.
     */
    template<class T>
    void removeComponent(ComponentHandle<T> handle)
    {
        const ComponentTypeId type_id = GetComponentTypeId<T>();
        
        if (!hasComponent(handle))
            throw std::runtime_error("Component doesn't belongs to this entity.");

        const auto iter = 
            std::find(components[type_id].begin(), components[type_id].end(), handle.raw);
        
        ECS::DeleteComponent(handle);
        components[type_id].erase(iter);
    }

    /** Delete every component owned by this entity.
     * 
     * Note: The deletion only occurs when `ECS::DeletePendings` is called (usually at the end of the frame).
     * 
     * Note: This function is automatically called by the game system when the entity is removed.
     */
    void clearAllComponents();
    
    void debugEntity();
    
    
private:
    std::unordered_map<ComponentTypeId, std::vector<StoredComponent>> components;
};
