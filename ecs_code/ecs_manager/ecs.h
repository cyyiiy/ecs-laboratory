#pragma once
#include "component.h"
#include "behaviorComponent.h"
#include "componentManager.h"


class ECS
{
private:
    static std::vector<IComponentManager*> managers;
    static std::vector<IBehaviorManager*> behavior_managers;
    
public:
    template <typename T>
    static ComponentManager<T, ComponentSublistSize<T>::value>& Manager()
    {
        constexpr std::size_t SublistSize = ComponentSublistSize<T>::value;
        
        if constexpr (std::is_base_of_v<BehaviorComponent, T>)
        {
            static BehaviorManager<T, SublistSize> behaviorManager;
            
            // Static local variable 'registered' allow the lambda to be executed only once for each component class
            static bool registered = []
            {
                managers.push_back(&behaviorManager);
                behavior_managers.push_back(&behaviorManager);
                return true;
            }();
        
            (void)registered; // Tell the compiler that it's normal to keep 'registered' unused
        
            return behaviorManager;
        }
        else
        {
            static ComponentManager<T, SublistSize> componentManager;
            
            // Static local variable 'registered' allow the lambda to be executed only once for each component class
            static bool registered = []
            {
                managers.push_back(&componentManager);
                return true;
            }();
        
            (void)registered; // Tell the compiler that it's normal to keep 'registered' unused
        
            return componentManager;
        }
    }
    
    template <typename T>
    static ComponentHandle CreateComponent(class Entity* ownerEntity)
    {
        return Manager<T>().CreateComponent(ownerEntity);
    }
    
    template <typename T>
    static void DeleteComponent(const ComponentHandle& handle)
    {
        Manager<T>().DeleteComponent(handle);
    }
    
    template <typename T>
    static T& GetComponent(const ComponentHandle& handle)
    {
        return Manager<T>().GetComponent(handle);
    }
    
    static void Update(float deltaTime)
    {
        for (IBehaviorManager* behavior_manager : behavior_managers)
        {
            behavior_manager->UpdateComponents(deltaTime);
        }
    }
    
    static void DeletePendings()
    {
        for (IComponentManager* manager : managers)
        {
            manager->DeletePendingComponents();
        }
    }
    
    static void Clear()
    {
        for (IComponentManager* manager : managers)
        {
            manager->ClearAllComponents();
        }
    }
    
    
    static void DebugECS();
};