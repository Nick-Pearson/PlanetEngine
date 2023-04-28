#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "ofbx.h"

#include "Mesh/Mesh.h"

class FBXImporter
{
 public:
    static std::shared_ptr<Mesh> Import(const char* filepath, float scaleFactor = 1.0f);

 private:
    FBXImporter(const ofbx::IScene* scene, float scale_factor);

    std::shared_ptr<Mesh> Import();

    void ImportVerts(const ofbx::Geometry* fbx_geo);
    void ImportIndexedItems(const ofbx::Geometry* fbx_geo);

    int RemapVertIdx(int idx, const struct Vector& normal, const struct Vector2D& uvs);

    const ofbx::IScene* scene_;
    const ofbx::GlobalSettings* settings_;
    float scale_factor_;

    std::vector<Vertex> verts_;
    std::vector<uint16_t> tris_;
    std::unordered_map<int, std::vector<int>> mapped_verts_;
};
