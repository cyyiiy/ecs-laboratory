#pragma once
#include "../ecs_manager/behaviorComponent.h"


class ComponentB : public BehaviorComponent
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