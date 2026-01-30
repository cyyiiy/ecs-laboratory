#pragma once

class Entity; // Forward declaration


template <class T>
struct ComponentSublistSize
{
    static constexpr size_t value = 64; // Default sublist size
};


class Component
{
public:
    Component() = default;
    virtual ~Component() = default;
    
    Component(const Component& other) = delete;
    Component& operator=(const Component&) = delete;
    
    void setUpdateActivated(bool value);
    bool getUpdateActivated() const;
    
    void setPendingDelete(bool value);
    bool getPendingDelete() const;
    
    void setOwner(Entity* ownerEntity);
    Entity* getOwner() const;
    
    /** Called after the component has been created. */
    virtual void init() {}
    
    /** Called each frame except if update has been disabled for this component. */
    virtual void update(float deltaTime) {}
    
    /** Called before the component is deleted. Note: the deletion happens at the end of the frame. */
    virtual void exit() {}
    
private:
    bool updateActivated{ true };
    bool pendingDelete{ false };
    
    Entity* owner{ nullptr };
};