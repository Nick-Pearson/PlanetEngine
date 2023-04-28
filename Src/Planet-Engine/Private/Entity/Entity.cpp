#include "Entity/Entity.h"

Entity::Entity()
{
}

Entity::~Entity()
{
    for (Component* component : components_)
    {
        delete component;
    }
    components_.clear();
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
    transform_.location += translation;
    OnTransformChanged();
}

void Entity::Rotate(Vector rotation)
{
    if (rotation.IsZero()) return;

    transform_.rotation *= Quaternion(rotation);
    OnTransformChanged();
}

void Entity::SetRotation(Quaternion rotation)
{
    transform_.rotation = rotation;
    OnTransformChanged();
}

void Entity::OnTransformChanged()
{
    for (Component* component : components_)
    {
        component->OnEntityTransformChanged();
    }
}
