#include "Entity/Component.h"
#include "Entity/Entity.h"

Component::Component()
{
}

Transform Component::GetWorldTransform() const
{
    return GetParent()->GetTransform();
    // * relativeTransform
}

void Component::OnSpawned()
{
}

void Component::OnDestroyed()
{
}

void Component::OnUpdate(float deltaSeconds)
{
}

void Component::OnEntityTransformChanged()
{
}
