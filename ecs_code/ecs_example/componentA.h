#pragma once
#include "../ecs_manager/behaviorComponent.h"


class ComponentA : public BehaviorComponent
{
public:
    void init() override;
    void update(float deltaTime) override;
    void exit() override;
    
    void setValue(const int val);
    void readValue();
    
private:
    int value{ 0 };
};


// Specify sublist size for 'ComponentA'
template <>
struct ComponentSublistSize<ComponentA>
{
    static constexpr size_t value = 8;
};