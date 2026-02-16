#pragma once
#include "component.h"
#include "behaviorComponent.h"
#include "componentManager.h"


class ECS
{
private:
    static std::vector<IComponentManager*> managers;
    static std::vector<IBehaviorManager*> behaviorManagers;
    
public:
    /** Get the component manager of a given component class.
     * 
     * @tparam T The component class.
     * @return A reference to the manager.
     */
    template <class T>
    static ComponentManager<T, ComponentSublistSize<T>::value>& Manager()
    {
        // Retrieve the sublist size of the given class (or the default one if the class didn't define one)
        constexpr std::size_t SublistSize = ComponentSublistSize<T>::value;
        
        // Register the manager in the static vectors on the first call to this function for each component class
        // Managers of components that inherit from BehaviorComponent are also registered in a separate vector for the Update loop
        if constexpr (std::is_base_of_v<BehaviorComponent, T>)
        {
            static BehaviorManager<T, SublistSize> behaviorManager;
            
            // Static local variable 'registered' allow the lambda to be executed only once for each component class
            static bool registered = []
            {
                managers.push_back(&behaviorManager);
                behaviorManagers.push_back(&behaviorManager);
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

    /** Creates a new component of a given class in the ECS.
     * 
     * @tparam T The component class.
     * @param ownerEntity The entity that owns this component. Must be nullptr if this function is not called from `Entity::addComponentByClass`.
     * @return A ComponentHandle that allows to retrieve and access the component.
     */
    template <class T>
    static ComponentHandle<T> CreateComponent(class Entity* ownerEntity = nullptr)
    {
        return Manager<T>().CreateComponent(ownerEntity);
    }

    /** Deletes an existing component from the ECS.
     * 
     * Note: The deletion only occurs when `ECS::DeletePendings` is called (usually at the end of the frame).
     * 
     * @tparam T The component class. | This function can be called without explicitly use the template argument as it is deduced from the ComponentHandle.
     * @param handle The ComponentHandle that allows to access the component to delete.
     */
    template <class T>
    static void DeleteComponent(const ComponentHandle<T>& handle)
    {
        Manager<T>().DeleteComponent(handle);
    }

    /** Deletes an existing component from the ECS, using a non-templated object RawComponentHandle.
     * 
     * Note: This function mainly exists so `Entity::clearAllComponents` function works. Prefer using `ECS::DeleteComponent` in most case.
     * 
     * @tparam T The component class.
     * @param rawHandle The RawComponentHandle that allows to access the component to delete.
     */
    template <class T>
    static void DeleteComponentRawHandle(const RawComponentHandle& rawHandle)
    {
        const ComponentHandle<T> handle(rawHandle);
        Manager<T>().DeleteComponent(handle);
    }

    /** Get a reference to an existing component in the ECS.
     * 
     * @tparam T The component class. | This function can be called without explicitly use the template argument as it is deduced from the ComponentHandle.
     * @param handle The ComponentHandle that allows to access the component to get.
     * @return A reference to the component.
     */
    template <class T>
    static T& GetComponent(const ComponentHandle<T>& handle)
    {
        return Manager<T>().GetComponent(handle);
    }

    /** Know if a component handle access to an existing component.
     * 
     * @tparam T The component class. | This function can be called without explicitly use the template argument as it is deduced from the ComponentHandle.
     * @param handle The ComponentHandle to check.
     * @return True if the component handle is valid, False otherwise.
     */
    template <class T>
    static bool IsComponentHandleValid(const ComponentHandle<T>& handle) noexcept
    {
        return Manager<T>().IsComponentHandleValid(handle);
    }

    /** Update all existing behavior components in the ECS.
     * 
     * @param deltaTime The duration of the frame.
     */
    static void Update(float deltaTime)
    {
        for (IBehaviorManager* behavior_manager : behaviorManagers)
        {
            behavior_manager->UpdateComponents(deltaTime);
        }
    }

    /** Delete all pending components in the ECS.
     */
    static void DeletePendings()
    {
        for (IComponentManager* manager : managers)
        {
            manager->DeletePendingComponents();
        }
    }

    /** Instantly delete every existing components in the ECS.
     */
    static void Clear()
    {
        for (IComponentManager* manager : managers)
        {
            manager->ClearAllComponents();
        }
    }
    
    
    static void DebugECS();
};