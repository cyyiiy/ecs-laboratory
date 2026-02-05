#pragma once
#include <string>
#include "../ecs_manager/component.h"


class ComponentDataA : public Component
{
public:
    int integer_value{ 0 };
    std::string string_value{ "" };
};