#include <iostream>
#include "ecs_manager/ecs.h"
#include "ecs_example/componentA.h"
#include "ecs_example/componentB.h"


void testComponents()
{
    std::cout << "===================================================\n";
    std::cout << "=============== ECS Test Components ===============\n";
    std::cout << "===================================================\n";
    
    std::cout << "\n==> Creating two components of class ComponentB:\n";
    ComponentHandle componentB_1 = ECS::CreateComponent<ComponentB>();
    ComponentHandle componentB_2 = ECS::CreateComponent<ComponentB>();
    ECS::DebugECS();
    
    std::cout << "\n==> Set and read values to created components:\n";
    ECS::GetComponent<ComponentB>(componentB_1).setValue(1);
    ECS::GetComponent<ComponentB>(componentB_2).setValue(2);
    ECS::GetComponent<ComponentB>(componentB_1).readValue();
    ECS::GetComponent<ComponentB>(componentB_2).readValue();
    
    std::cout << "\n==> Execute ECS Update:\n";
    ECS::Update(0.1f);
    
    std::cout << "\n==> Delete one of the created components:\n";
    ECS::DeleteComponent<ComponentB>(componentB_1); // Set pending delete, not immediately
    ECS::DebugECS();
    
    std::cout << "\n==> Execute ECS Delete Pendings:\n";
    ECS::DeletePendings();
    ECS::DebugECS();
    
    std::cout << "\n==> Execute ECS Update:\n";
    ECS::Update(0.1f);
    
    std::cout << "\n==> Create two more components of class ComponentB:\n";
    ECS::CreateComponent<ComponentB>();
    ECS::CreateComponent<ComponentB>();
    ECS::DebugECS();
    
    std::cout << "\n==> Create ten components of class ComponentA:\n";
    for (int i = 0; i < 10; i++)
        ECS::CreateComponent<ComponentA>();
    ECS::DebugECS();
    
    std::cout << "\n==> Execute ECS Update:\n";
    ECS::Update(0.1f);
    
    std::cout <<"\n==> Execute ECS Clear:\n";
    ECS::Clear();
    ECS::DebugECS();
}

int main()
{
    testComponents();
    
    return 0;
}