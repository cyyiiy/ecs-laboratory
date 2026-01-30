#include "component.h"
#include "entity.h"

void Component::setUpdateActivated(bool value)
{
    updateActivated = value;
}

bool Component::getUpdateActivated() const
{
    return updateActivated;
}

void Component::setPendingDelete(bool value)
{
    pendingDelete = value;
}

bool Component::getPendingDelete() const
{
    return pendingDelete;
}

void Component::setOwner(Entity* ownerEntity)
{
    owner = ownerEntity;
}

Entity* Component::getOwner() const
{
    return owner;
}
