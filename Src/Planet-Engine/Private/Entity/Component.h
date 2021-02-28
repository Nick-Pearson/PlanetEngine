#pragma once

#include "Math/Transform.h"

class Entity;
class Scene;

__declspec(align(16))
class Component
{
    friend class Entity;
 public:
    Component();

    // alignment for Direct X structures
    void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }

    inline Entity* GetParent() const { return parent; }

    Transform GetWorldTransform();

    virtual void OnSpawned();
    virtual void OnDestroyed();
    virtual void OnUpdate(float deltaSeconds);
    virtual void OnEntityTransformChanged();

 private:
    Entity* parent = nullptr;

    // Transform relativeTransform;
};