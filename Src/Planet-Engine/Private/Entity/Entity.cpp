#include "Entity/Entity.h"

Entity::Entity()
{
}

Entity::~Entity()
{
    for (Component* component : components)
    {
        delete component;
    }
    components.clear();
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

    transform.rotation *= Quaternion(rotation);
    OnTransformChanged();
}

void Entity::SetRotation(Quaternion rotation)
{
    transform.rotation = rotation;
    OnTransformChanged();
}

void Entity::OnTransformChanged()
{
    for (Component* component : components)
    {
        component->OnEntityTransformChanged();
    }
}
