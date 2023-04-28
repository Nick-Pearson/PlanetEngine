#pragma once

#include <memory>
#include <vector>

#include "Math/Transform.h"
#include "Component.h"

class Scene;

// an object that exists within the scene
__declspec(align(16))
class Entity
{
    friend class Scene;
 public:
    float rotateSpeed = 0.0f;
    Entity();
    ~Entity();

    // alignment for Direct X structures
    void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }

    template<class T, typename... Args>
    T* AddComponent(Args... args);

    virtual void OnSpawned();
    virtual void OnDestroyed();
    virtual void OnUpdate(float deltaSeconds);

    inline Scene* GetScene() const { return scene_; }

    inline Transform GetTransform() const { return transform_; }

    inline const char* GetName() { return name_; }
    inline void SetName(const char* name) { name_ = name; }

    void Translate(Vector translation);
    void Rotate(Vector rotation);

    inline Quaternion GetRotation() const { return transform_.rotation; }
    void SetRotation(Quaternion rotation);

 public:
    Transform transform_;

    std::vector<Component*> components_;

 private:
    void OnTransformChanged();

    const char* name_ = nullptr;
    Scene* scene_ = nullptr;
};

template<class T, typename... Args>
T* Entity::AddComponent(Args... args)
{
    T* ptr = new T(args...);
    components_.push_back(ptr);

    ptr->parent = this;
    ptr->OnSpawned();
    return ptr;
}
