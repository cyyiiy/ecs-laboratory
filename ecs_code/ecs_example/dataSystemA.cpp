#include "dataSystemA.h"
#include "componentDataA.h"
#include "../ecs_manager/ecs.h"
#include <iostream>

void DataSystemA::updateSystem()
{
    std::cout << "====== DEBUG DATA SYSTEM ======\n";
    auto& data_components = ECS::Manager<ComponentDataA>();
    data_components.ForEach([](const ComponentDataA& component)
    {
        std::cout << "Data Component: Integer value: " << component.integer_value << " | String value: " << component.string_value << "\n";
    });
    std::cout << "===============================\n";
}
