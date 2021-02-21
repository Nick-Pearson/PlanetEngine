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
	std::shared_ptr<T> AddComponent(Args... args);

	virtual void OnSpawned();
	virtual void OnDestroyed();
	virtual void OnUpdate(float deltaSeconds);

	inline Scene* GetScene() const { return scene; }

	inline Transform GetTransform() const { return transform; }

	inline const char* GetName() { return mName; }
	inline void SetName(const char* name) { mName = name; }

	void Translate(Vector translation);
	void Rotate(Vector rotation);

public:
	Transform transform;

	std::vector<std::shared_ptr<Component>> components;

private:

	void OnTransformChanged();

	const char* mName = nullptr;
	Scene* scene = nullptr;
};

template<class T, typename... Args>
std::shared_ptr<T> Entity::AddComponent(Args... args)
{
	T* ptr = new T(args...);
	std::shared_ptr<T> newcomp = std::shared_ptr<T>(ptr);
	components.push_back(newcomp);

	newcomp->parent = this;
	newcomp->OnSpawned();
	return newcomp;
}
