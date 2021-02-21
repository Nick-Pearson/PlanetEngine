#include "Scene.h"
#include "imgui.h"

Scene::Scene()
{

}

void Scene::Update(float deltaSeconds)
{
	for (std::shared_ptr<Entity> entity : entities)
	{
		entity->OnUpdate(deltaSeconds);
	}
}
