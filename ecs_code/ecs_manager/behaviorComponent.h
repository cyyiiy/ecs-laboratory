#pragma once
#include "component.h"

/**
 * Specialized component class that provides `init`, `update` and `exit` functions automatically called by the ECS.
 * 
 * Also provides the ability to enable/disable the update for each component.
 */
class BehaviorComponent : public Component
{
public:
    BehaviorComponent() = default;
    BehaviorComponent(const BehaviorComponent& other) = default;
    
    BehaviorComponent& operator=(const BehaviorComponent&) = delete;
    
    void setUpdateActivated(bool value);
    bool getUpdateActivated() const;
    
    /** Called after the component has been created. */
    virtual void init() {}
    
    /** Called each frame except if update has been disabled for this component. */
    virtual void update(float deltaTime) {}
    
    /** Called before the component is deleted. Note: the deletion happens at the end of the frame. */
    virtual void exit() {}
    
private:
    bool updateActivated{ true };
};
