#include <iostream>
#include "ecs_manager/ecs.h"
#include "ecs_manager/entity.h"
#include "ecs_example/componentA.h"
#include "ecs_example/componentB.h"
#include "ecs_example/componentDataA.h"
#include "ecs_example/dataSystemA.h"


void testComponents()
{
    std::cout << "===================================================\n";
    std::cout << "=============== ECS Test Components ===============\n";
    std::cout << "===================================================\n";
    
    std::cout << "\n==> Create two components of class ComponentB:\n";
    ComponentHandle<ComponentB> componentB_1 = ECS::CreateComponent<ComponentB>();
    ComponentHandle<ComponentB> componentB_2 = ECS::CreateComponent<ComponentB>();
    ECS::DebugECS();
    
    std::cout << "\n==> Set and read values to created components:\n";
    ECS::GetComponent(componentB_1).setValue(1);
    ECS::GetComponent(componentB_2).setValue(2);
    ECS::GetComponent(componentB_1).readValue();
    ECS::GetComponent(componentB_2).readValue();
    
    std::cout << "\n==> Execute ECS Update:\n";
    ECS::Update(0.1f);
    
    std::cout << "\n==> Test equality between component handle and self handle of a component:\n";
    ComponentB& componentB_1_ref = ECS::GetComponent(componentB_1);
    std::cout << "Equality of 'componentB_1' and the self handle of the component: " << (componentB_1 == componentB_1_ref.getSelfHandle<ComponentB>());
    
    std::cout << "\n==> Delete one of the created components:\n";
    ECS::DeleteComponent(componentB_1); // Set pending delete, not immediately
    ECS::DebugECS();
    
    std::cout << "\n==> Execute ECS Delete Pendings:\n";
    ECS::DeletePendings();
    ECS::DebugECS();
    
    std::cout << "\n==> Test validity of component handles:\n";
    std::cout << "ComponentHandle 'componentB_1': " << ECS::IsComponentHandleValid(componentB_1) << "\n";
    std::cout << "ComponentHandle 'componentB_2': " << ECS::IsComponentHandleValid(componentB_2) << "\n";
    
    std::cout << "\n==> Execute ECS Update:\n";
    ECS::Update(0.1f);
    
    std::cout << "\n==> Create two more components of class ComponentB:\n";
    ComponentHandle<ComponentB> componentB_3 = ECS::CreateComponent<ComponentB>();
    ComponentHandle<ComponentB> componentB_4 = ECS::CreateComponent<ComponentB>();
    ECS::DebugECS();
    
    std::cout << "\n==> Read values of created components (one is a reused from the one deleted earlier and one is brand new):\n";
    ECS::GetComponent(componentB_3).readValue();
    ECS::GetComponent(componentB_4).readValue();
    
    std::cout << "\n==> Create a component of class ComponentA and test the validity of its owned handle:\n";
    ComponentHandle<ComponentA> componentA = ECS::CreateComponent<ComponentA>();
    ComponentA& componentA_ref = ECS::GetComponent(componentA);
    std::cout << "ComponentHandle 'testHande' of 'componentA': " << ECS::IsComponentHandleValid(componentA_ref.testHandle) << "\n";
    
    std::cout << "\n==> Create ten other components of class ComponentA:\n";
    for (int i = 0; i < 10; i++)
        ECS::CreateComponent<ComponentA>();
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
    ComponentHandle<ComponentA> handle = entity1.addComponentByClass<ComponentA>();
    entity1.debugEntity();
    ECS::DebugECS();
    
    std::cout << "\n==> Manipulate the component of the entity:\n";
    ComponentHandle<ComponentA> handle_bis = entity1.getComponentOfClass<ComponentA>(); // handle_bis == handle
    ComponentA& component = entity1.getComponent(handle_bis);
    component.setValue(1);
    component.readValue();
    
    std::cout << "\n==> Create another entity with components:\n";
    Entity entity2;
    entity2.addComponentByClass<ComponentA>();
    entity2.addComponentByClass<ComponentB>();
    entity2.addComponentByClass<ComponentB>();
    entity2.addComponentByClass<ComponentDataA>();
    entity2.debugEntity();
    ECS::DebugECS();
    
    std::cout << "\n==> Clear components on this entity and execute ECS Delete Pendings:\n";
    entity2.clearAllComponents();
    ECS::DeletePendings();
    ECS::DebugECS();
    
    std::cout << "\n==> Remove the component from the first entity:\n";
    entity1.removeComponent(handle);
    entity1.debugEntity();
    
    std::cout << "\n==> Execute ECS Delete Pendings:\n";
    ECS::DeletePendings();
    ECS::DebugECS();
}

void testDataSystem()
{
    std::cout << "===================================================\n";
    std::cout << "=============== ECS Test Data System ==============\n";
    std::cout << "===================================================\n";
    
    DataSystemA data_system;
    
    std::cout << "\n==> Test data system with 0 created components:\n";
    data_system.updateSystem();
    
    std::cout << "\n==> Create three data components:\n";
    ComponentHandle<ComponentDataA> component_data_1 = ECS::CreateComponent<ComponentDataA>();
    ComponentHandle<ComponentDataA> component_data_2 = ECS::CreateComponent<ComponentDataA>();
    ComponentHandle<ComponentDataA> component_data_3 = ECS::CreateComponent<ComponentDataA>();
    ECS::DebugECS();
    
    std::cout << "\n==> Set values to them:\n";
    ECS::GetComponent(component_data_1).integer_value = 111;
    ECS::GetComponent(component_data_1).string_value = "AAA";
    ECS::GetComponent(component_data_2).integer_value = 222;
    ECS::GetComponent(component_data_2).string_value = "BBB";
    ECS::GetComponent(component_data_3).integer_value = 333;
    ECS::GetComponent(component_data_3).string_value = "CCC";
    data_system.updateSystem();
    
    std::cout << "\n==> Delete the second component:\n";
    ECS::DeleteComponent<ComponentDataA>(component_data_2);
    ECS::DeletePendings();
    ECS::DebugECS();
    data_system.updateSystem();
    
    std::cout << "\n==> Create two new components:\n";
    ComponentHandle<ComponentDataA> component_data_4 = ECS::CreateComponent<ComponentDataA>();
    ComponentHandle<ComponentDataA> component_data_5 = ECS::CreateComponent<ComponentDataA>();
    ECS::DebugECS();
    
    std::cout << "\n==> Set values to the new components:\n";
    ECS::GetComponent(component_data_4).integer_value = 444;
    ECS::GetComponent(component_data_4).string_value = "DDD";
    ECS::GetComponent(component_data_5).integer_value = 555;
    ECS::GetComponent(component_data_5).string_value = "EEE";
    data_system.updateSystem();
    
    std::cout << "\n==> Clear the components:\n";
    ECS::Clear();
    ECS::DebugECS();
    data_system.updateSystem();
}

int main()
{
    //testComponents();
    //testEntities();
    testDataSystem();
    
    return 0;
}