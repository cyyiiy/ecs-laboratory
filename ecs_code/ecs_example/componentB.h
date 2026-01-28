#pragma once
#include "../ecs_manager/component.h"


class ComponentB : public Component
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