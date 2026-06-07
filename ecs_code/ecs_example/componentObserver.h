#pragma once
#include "../ecs_manager/component.h"
#include "../event_system/observer.h"
#include <string>


class ComponentObserver : public Component, public Observer
{
public:
    void callbackFunction(std::string str, int num);
};