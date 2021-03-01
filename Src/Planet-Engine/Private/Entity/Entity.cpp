#include "Entity/Entity.h"

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
    Rotate(Vector{ 0.0f, rotateSpeed * deltaSeconds, 0.0f });
}

void Entity::Translate(Vector translation)
{
    transform.location += translation;
    OnTransformChanged();
}

void Entity::Rotate(Vector rotation)
{
    if (rotation.IsZero()) return;

    transform.rotation += rotation;
    OnTransformChanged();
}

void Entity::OnTransformChanged()
{
    for (std::shared_ptr<Component> component : components)
    {
        component->OnEntityTransformChanged();
    }
}
