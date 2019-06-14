#pragma once

#include <vector>
#include <memory>

#include "../Entity/Entity.h"

class Scene
{
public:
	Scene();

	template<class T = Entity, typename... Args>
	std::shared_ptr<T> SpawnEntity(Args... args);

	void Update(float deltaSeconds);

private:

	std::vector<std::shared_ptr<Entity>> entities;
};

template<class T /*= Entity*/, typename... Args>
std::shared_ptr<T> Scene::SpawnEntity(Args... args)
{
	T* ptr = new T(args...);
	std::shared_ptr<T> newEntity = std::shared_ptr<T>(ptr);
	entities.push_back(newEntity);

	newEntity->scene = this;
	newEntity->OnSpawned();
	return newEntity;
}