#include "Entity.h"

Entity::Entity()
{

}

void Entity::OnSpawned()
{

}

void Entity::OnDestroyed()
{

}

void Entity::OnUpdate(float deltaSeconds)
{

}

void Entity::Translate(Vector translation)
{
	transform.location += translation;
}
