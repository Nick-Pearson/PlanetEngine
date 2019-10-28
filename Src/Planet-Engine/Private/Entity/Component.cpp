#include "Component.h"
#include "Entity.h"

Component::Component()
{

}

Transform Component::GetWorldTransform()
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
