#include <iostream>
#include "ecs_manager/ecs.h"
#include "ecs_manager/entity.h"
#include "ecs_example/componentA.h"
#include "ecs_example/componentB.h"
#include "ecs_example/componentDataA.h"


void testComponents()
{
    std::cout << "===================================================\n";
    std::cout << "=============== ECS Test Components ===============\n";
    std::cout << "===================================================\n";
    
    std::cout << "\n==> Create two components of class ComponentB:\n";
    ComponentHandle componentB_1 = ECS::CreateComponent<ComponentB>(nullptr);
    ComponentHandle componentB_2 = ECS::CreateComponent<ComponentB>(nullptr);
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
    ComponentHandle componentB_3 = ECS::CreateComponent<ComponentB>(nullptr);
    ComponentHandle componentB_4 = ECS::CreateComponent<ComponentB>(nullptr);
    ECS::DebugECS();
    
    std::cout << "\n==> Read values of created components (one is a reused from the one deleted earlier and one is brand new):\n";
    ECS::GetComponent<ComponentB>(componentB_3).readValue();
    ECS::GetComponent<ComponentB>(componentB_4).readValue();
    
    std::cout << "\n==> Create ten components of class ComponentA:\n";
    for (int i = 0; i < 10; i++)
        ECS::CreateComponent<ComponentA>(nullptr);
    ECS::DebugECS();
    
    std::cout << "\n==> Execute ECS Update:\n";
    ECS::Update(0.1f);
    
    std::cout <<"\n==> Execute ECS Clear:\n";
    ECS::Clear();
    ECS::DebugECS();
}

void testEntities()
{
    std::cout << "===================================================\n";
    std::cout << "================ ECS Test Entities ================\n";
    std::cout << "===================================================\n";
    
    std::cout << "\n==> Create an entity and a component on it:\n";
    Entity entity1;
    ComponentHandle handle = entity1.addComponentByClass<ComponentA>();
    entity1.debugEntity();
    ECS::DebugECS();
    
    std::cout << "\n==> Manipulate the component of the entity:\n";
    ComponentA* component = entity1.getComponent<ComponentA>(handle);
    component = entity1.getComponentOfClass<ComponentA>(); // Both gives the same result
    component->setValue(1);
    component->readValue();
    
    std::cout << "\n==> Create another entity with components inside a scope:\n";
    {
        Entity entity2;
        entity2.addComponentByClass<ComponentA>();
        entity2.addComponentByClass<ComponentB>();
        entity2.addComponentByClass<ComponentB>();
        entity2.addComponentByClass<ComponentDataA>();
        entity2.debugEntity();
        ECS::DebugECS();
    }
    
    std::cout << "\n==> Exit the scope and execute ECS Delete Pendings:\n";
    ECS::DeletePendings();
    ECS::DebugECS();
    
    std::cout << "\n==> Remove the component from the first entity:\n";
    entity1.removeComponent<ComponentA>(handle);
    entity1.debugEntity();
    
    std::cout << "\n==> Execute ECS Delete Pendings:\n";
    ECS::DeletePendings();
    ECS::DebugECS();
}

int main()
{
    //testComponents();
    testEntities();
    
    return 0;
}