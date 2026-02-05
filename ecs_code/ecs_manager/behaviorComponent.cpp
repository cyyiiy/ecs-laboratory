#include "behaviorComponent.h"

void BehaviorComponent::setUpdateActivated(bool value)
{
    updateActivated = value;
}

bool BehaviorComponent::getUpdateActivated() const
{
    return updateActivated;
}
