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

private:

	std::vector<std::shared_ptr<Entity>> entities;
};

template<class T /*= Entity*/, typename... Args>
std::shared_ptr<T> Scene::SpawnEntity(Args... args)
{
	std::shared_ptr<T> newEntity = std::make_shared<T>();
	entities.push_back(newEntity);

	newEntity->scene = this;
	newEntity->OnSpawned();
	return newEntity;
}