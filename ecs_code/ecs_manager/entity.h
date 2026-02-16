#pragma once
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "ecsTypes.h"
#include "ecs.h"
#include "component.h"


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

class Entity
{
public:
    Entity() = default;
    ~Entity();
    
    Entity(const Entity& other) = delete;
    Entity& operator=(const Entity&) = delete;
    
    
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
    
    template<class T>
    T* getComponent(ComponentHandle<T> handle)
    {
        if (!hasComponent(handle))
            throw std::runtime_error("Component doesn't belongs to this entity.");
        
        return &ECS::GetComponent(handle);
    }
    
    template<class T>
    bool hasComponentOfClass() const noexcept
    {
        const ComponentTypeId type_id = GetComponentTypeId<T>();
        
        return components.find(type_id) != components.end() && components.at(type_id).size() > 0;
    }
    
    template<class T>
    T* getComponentOfClass()
    {
        if (!hasComponentOfClass<T>())
            throw std::runtime_error("Entity doesn't have a component of this class.");
        
        const ComponentTypeId type_id = GetComponentTypeId<T>();
        const ComponentHandle<T> handle(components[type_id][0].raw_handle);
        
        return &ECS::GetComponent(handle);
    }
    
    template<class T>
    std::vector<T*> getAllComponentsOfClass()
    {
        const ComponentTypeId type_id = GetComponentTypeId<T>();
        
        if (components.find(type_id) == components.end())
            throw std::runtime_error("Entity doesn't have a component of this class.");
        
        if (components[type_id].size() == 0)
            throw std::runtime_error("Entity doesn't have a component of this class.");
        
        std::vector<T*> return_list;
        return_list.reserve(components[type_id].size());
        
        for (StoredComponent stored_handle : components[type_id])
        {
            const ComponentHandle<T> handle(stored_handle.raw_handle);
            return_list.push_back(&ECS::GetComponent(handle));
        }
        
        return return_list;
    }
    
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
    
    void clearAllComponents();
    
    void debugEntity();
    
    
private:
    std::unordered_map<ComponentTypeId, std::vector<StoredComponent>> components;
};
