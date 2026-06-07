#include "componentObserver.h"
#include <iostream>

void ComponentObserver::callbackFunction(std::string str, int num)
{
    ComponentHandle<ComponentObserver> handle = getSelfHandle<ComponentObserver>();
    
    std::cout << 
        "ComponentObserver from Callback Function: \n"
        "Handle: Sublist " << handle.raw.sublistId << " | Slot " << handle.raw.slotId << " | Generation " << handle.raw.generation << "\n"
        "Address: " << this << "\n"
        "Callback parameters: " << str << " | " << num << "\n";
}
