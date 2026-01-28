#include "component.h"

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
