#pragma once

#include <vector>

class MeshComponent;

struct RenderQueueItems
{
    std::vector<const MeshComponent*> new_meshes;
    std::vector<const MeshComponent*> updated_meshes;
    std::vector<const MeshComponent*> removed_meshes;
};

// Queue of changes to the scene
// Changes are rendered in the next frame
class RenderQueue
{
 public:
    void AddMesh(const MeshComponent* mesh);
    void UpdateMesh(const MeshComponent* mesh);
    void RemoveMesh(const MeshComponent* mesh);

    inline const RenderQueueItems& GetItems() const { return items; }
    void ClearQueue();

 private:
    RenderQueueItems items;
};