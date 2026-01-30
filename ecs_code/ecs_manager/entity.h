#pragma once
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>

#include "ecsTypes.h"
#include "ecs.h"


struct StoredComponent
{
    ComponentHandle handle;
    std::function<void(ComponentHandle)> deleteFunction;
    
    // Custom == operator to allow std::find to work by searching a ComponentHandle in a list of StoredComponent
    bool operator==(const ComponentHandle& otherHandle) const
    {
        return otherHandle == this->handle;
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
    std::enable_if_t<std::is_base_of<Component, T>::value, ComponentHandle>
        addComponentByClass()
    {
        ComponentTypeId type_id = GetComponentTypeId<T>();
        ComponentHandle handle = ECS::CreateComponent<T>();
        
        components[type_id].push_back(
            StoredComponent{
                handle,
                &ECS::DeleteComponent<T>
            });
        
        return handle;
    }
    
    template<class T>
    std::enable_if_t<std::is_base_of<Component, T>::value, T*>
        getComponent(ComponentHandle handle)
    {
        ComponentTypeId type_id = GetComponentTypeId<T>();
        
        if (components.find(type_id) == components.end())
            throw std::runtime_error("Component doesn't belongs to this entity.");

        const auto iter = 
            std::find(components[type_id].begin(), components[type_id].end(), handle);
        
        if (iter == components[type_id].end())
            throw std::runtime_error("Component doesn't belongs to this entity.");
        
        return &ECS::GetComponent<T>(iter->handle);
    }
    
    template<class T>
    std::enable_if_t<std::is_base_of<Component, T>::value, bool>
        hasComponentOfClass()
    {
        ComponentTypeId type_id = GetComponentTypeId<T>();
        
        return components.find(type_id) != components.end() && components[type_id].size() > 0;
    }
    
    template<class T>
    std::enable_if_t<std::is_base_of<Component, T>::value, T*>
        getComponentOfClass()
    {
        ComponentTypeId type_id = GetComponentTypeId<T>();
        
        if (components.find(type_id) == components.end())
            throw std::runtime_error("Entity doesn't have a component of this class.");
        
        if (components[type_id].size() == 0)
            throw std::runtime_error("Entity doesn't have a component of this class.");
        
        return &ECS::GetComponent<T>(components[type_id][0].handle);
    }
    
    template<class T>
    std::enable_if_t<std::is_base_of<Component, T>::value, std::vector<T*>>
        getAllComponentsOfClass()
    {
        ComponentTypeId type_id = GetComponentTypeId<T>();
        
        if (components.find(type_id) == components.end())
            throw std::runtime_error("Entity doesn't have a component of this class.");
        
        if (components[type_id].size() == 0)
            throw std::runtime_error("Entity doesn't have a component of this class.");
        
        std::vector<T*> return_list;
        return_list.reserve(components[type_id].size());
        
        for (StoredComponent component : components[type_id])
        {
            return_list.push_back(&ECS::GetComponent<T>(component.handle));
        }
        
        return return_list;
    }
    
    template<class T>
    std::enable_if_t<std::is_base_of<Component, T>::value, void>
        removeComponent(ComponentHandle handle)
    {
        ComponentTypeId type_id = GetComponentTypeId<T>();
        
        if (components.find(type_id) == components.end())
            throw std::runtime_error("Component doesn't belongs to this entity.");

        const auto iter = 
            std::find(components[type_id].begin(), components[type_id].end(), handle);
        
        if (iter == components[type_id].end())
            throw std::runtime_error("Component doesn't belongs to this entity.");
        
        ECS::DeleteComponent<T>(handle);
        components[type_id].erase(iter);
    }
    
    void clearAllComponents();
    
    void debugEntity();
    
    
private:
    std::unordered_map<ComponentTypeId, std::vector<StoredComponent>> components;
};
