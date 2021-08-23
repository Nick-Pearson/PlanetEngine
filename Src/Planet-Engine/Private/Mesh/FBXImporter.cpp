#include "FBXImporter.h"

#include <cstdio>
#include <vector>
#include <utility>
#include "ofbx.h"

#include "PlanetLogging.h"
#include "Mesh/Mesh.h"

Vector toVector(const ofbx::Vec3& vec, const ofbx::GlobalSettings* settings)
{
    float x = static_cast<float>(vec.x);
    float y = static_cast<float>(vec.y);
    float z = static_cast<float>(vec.z);

    if (settings->CoordAxis == ofbx::CoordSystem_LeftHanded)
    {
        std::swap(x, y);
    }

    if (settings->UpAxis == ofbx::UpVector_AxisX)
    {
        std::swap(z, x);
        std::swap(y, x);
    }
    else if (settings->UpAxis == ofbx::UpVector_AxisZ)
    {
        std::swap(z, y);
        std::swap(y, x);
    }

    return Vector{x, y, z};
}

std::shared_ptr<Mesh> FBXImporter::Import(const char* filepath, float scaleFactor)
{
    FILE* fp = fopen(filepath, "rb");

    if (!fp) return nullptr;

    fseek(fp, 0, SEEK_END);
    int64_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    auto* content = new ofbx::u8[file_size];
    fread(content, 1, file_size, fp);
    ofbx::IScene* scene = ofbx::load((ofbx::u8*)content, static_cast<int>(file_size), (ofbx::u64)0);
    if (!scene)
    {
        P_FATAL("Failed loading FBX: {}", ofbx::getError());
    }

    const ofbx::GlobalSettings* settings = scene->getGlobalSettings();

    int geo_count = scene->getGeometryCount();
    P_ASSERT(geo_count == 1, "Multiple meshes in single file not supported, got {} meshes", geo_count);
    const ofbx::Geometry* fbx_geo = scene->getGeometry(0);

    int vertex_count = fbx_geo->getVertexCount();
    P_ASSERT(vertex_count < 65536, "Too many verticies, {}", vertex_count);
    const ofbx::Vec3* position_data = fbx_geo->getVertices();
    const ofbx::Vec3* normal_data = fbx_geo->getNormals();

    std::vector<Vertex> verts;
    verts.reserve(vertex_count);

    for (int i = 0; i < vertex_count; ++i)
    {
        const auto position = toVector(position_data[i], settings);
        auto normal = toVector(normal_data[i], settings);
        normal.Normalise();

        Vertex vert{ position, normal };
        verts.push_back(vert);
    }

    int index_count = fbx_geo->getIndexCount();
    const int* index_data = fbx_geo->getFaceIndices();

    std::vector<uint16_t> tris;
    tris.reserve(index_count);

    int i = 0;
    int count = 0;
    while (i < index_count)
    {
        int d = index_data[i];
        int idx = (d < 0) ? (-d - 1) : d;

        tris.push_back(idx);
        count++;

        if (d < 0)
        {
            if (count == 4)
            {
                // convert quad to triangle
                int x = tris[tris.size() - 2];
                int y = tris[tris.size() - 4];
                tris.push_back(y);
                tris.push_back(x);
            }
            count = 0;
        }

        i++;
    }

    return std::make_shared<Mesh>(verts, tris);
}