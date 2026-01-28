#include "componentB.h"
#include <iostream>

void ComponentB::init()
{
    std::cout << "ComponentB::init()" << std::endl;
}

void ComponentB::update(float deltaTime)
{
    std::cout << "ComponentB::update(" << deltaTime << ")" << std::endl;
}

void ComponentB::exit()
{
    std::cout << "ComponentB::exit()" << std::endl;
}

void ComponentB::setValue(const int val)
{
    value = val;
}

void ComponentB::readValue()
{
    std::cout << "ComponentB value is " << value << std::endl;
}
