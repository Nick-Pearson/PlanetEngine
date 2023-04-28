#include "Render/RenderQueue.h"

void RenderQueue::AddMesh(const MeshComponent* mesh)
{
    items.new_meshes.push_back(mesh);
}

void RenderQueue::UpdateMesh(const MeshComponent* mesh)
{
    items.updated_meshes.push_back(mesh);
}

void RenderQueue::RemoveMesh(const MeshComponent* mesh)
{
    items.removed_meshes.push_back(mesh);
}

void RenderQueue::ClearQueue()
{
    items.new_meshes.clear();
    items.updated_meshes.clear();
    items.removed_meshes.clear();
}