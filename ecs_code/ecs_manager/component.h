#pragma once
#include "ecsTypes.h"

class Entity; // Forward declaration


/**
 * Templated struct that contains the number of components contained per sublist for a class.
 * If this struct is defined for a specific class with a new value, it will override the default value of 64 for this class.
 * 
 * @tparam T The component class.
 */
template <class T>
struct ComponentSublistSize
{
    static constexpr size_t value = 64; // Default sublist size
};


/**
 * Base class for all components.
 * 
 * Store a pointer towards its owner `Entity` and knows if it is pending deletion.
 */
class Component
{
public:
    Component() = default;
    virtual ~Component() = default;
    Component(const Component& other) = default;
    
    Component& operator=(const Component&) = delete;
    
    void setPendingDelete(bool value);
    bool getPendingDelete() const;
    
    void setOwner(Entity* ownerEntity);
    Entity* getOwner() const;
    
    void setRawHandle(RawComponentHandle handle);
    template<class T>
    ComponentHandle<T> getSelfHandle() const
    {
        return ComponentHandle<T>(rawHandle);
    }
    
private:
    bool pendingDelete{ false };
    Entity* owner{ nullptr };
    RawComponentHandle rawHandle;
};