#include "ecs.h"
#include <iostream>

std::vector<IComponentManager*> ECS::managers;
std::vector<IBehaviorManager*> ECS::behaviorManagers;


void ECS::DebugECS()
{
    std::cout << "========== DEBUG ECS ==========\n";
    for (size_t i = 0; i < managers.size(); i++)
    {
        if (i > 0) std::cout << "--------------------\n";
        std::cout << managers[i]->DebugComponentManager();
    }
    std::cout << "===============================\n";
}
