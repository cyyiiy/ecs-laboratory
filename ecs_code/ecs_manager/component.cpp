#include "component.h"
#include "entity.h"

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

void Component::setRawHandle(RawComponentHandle handle)
{
    rawHandle = handle;
}
