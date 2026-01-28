#include "componentA.h"
#include <iostream>

void ComponentA::init()
{
    std::cout << "ComponentA::init()" << std::endl;
}

void ComponentA::update(float deltaTime)
{
    std::cout << "ComponentA::update(" << deltaTime << ")" << std::endl;
}

void ComponentA::exit()
{
    std::cout << "ComponentA::exit()" << std::endl;
}

void ComponentA::setValue(const int val)
{
    value = val;
}

void ComponentA::readValue()
{
    std::cout << "ComponentA value is " << value << std::endl;
}
