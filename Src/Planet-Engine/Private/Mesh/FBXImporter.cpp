#include "FBXImporter.h"

#include <cstdio>
#include <vector>
#include <utility>

#include "PlanetLogging.h"

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

Vector2D toVector(const ofbx::Vec2& vec, const ofbx::GlobalSettings* settings)
{
    float x = static_cast<float>(vec.x);
    float y = static_cast<float>(vec.y);

    if (settings->CoordAxis == ofbx::CoordSystem_LeftHanded)
    {
        std::swap(x, y);
    }

    return Vector2D{x, y};
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

    FBXImporter importer = FBXImporter{scene, scaleFactor};
    std::shared_ptr<Mesh> mesh = importer.Import();

    delete[] content;
    fclose(fp);

    return mesh;
}

FBXImporter::FBXImporter(const ofbx::IScene* scene, float scale_factor) :
    scene_(scene), scale_factor_(scale_factor)
{
    settings_ = scene_->getGlobalSettings();
}

std::shared_ptr<Mesh> FBXImporter::Import()
{
    int geo_count = scene_->getGeometryCount();
    P_ASSERT(geo_count == 1, "Multiple meshes in single file not supported, got {} meshes", geo_count);
    const ofbx::Geometry* fbx_geo = scene_->getGeometry(0);

    ImportVerts(fbx_geo);
    ImportIndexedItems(fbx_geo);

    return std::make_shared<Mesh>(verts_, tris_);
}

void FBXImporter::ImportVerts(const ofbx::Geometry* fbx_geo)
{
    int vertex_count = fbx_geo->getVertexCount();
    const ofbx::Vec3* position_data = fbx_geo->getVertices();
    int total_verts = verts_.size() + vertex_count;
    P_ASSERT(total_verts < 65536, "Too many verticies, {}", vertex_count);
    verts_.reserve(total_verts);

    for (int i = 0; i < vertex_count; ++i)
    {
        const auto position = toVector(position_data[i], settings_);

        Vertex vert{ position };
        verts_.push_back(vert);
    }
}

void FBXImporter::ImportIndexedItems(const ofbx::Geometry* fbx_geo)
{
    int index_count = fbx_geo->getIndexCount();
    const int* index_data = fbx_geo->getFaceIndices();
    const ofbx::Vec3* normal_data = fbx_geo->getNormals();
    const ofbx::Vec2* uv_data = fbx_geo->getUVs();

    int total_tris = tris_.size() + index_count;
    tris_.reserve(total_tris);

    int i = 0;
    int count = 0;
    while (i < index_count)
    {
        int d = index_data[i];
        int idx = (d < 0) ? (-d - 1) : d;

        auto normal = toVector(normal_data[i], settings_);
        normal.Normalise();
        normal *= -1;
        auto uv = toVector(uv_data[i], settings_);

        idx = RemapVertIdx(idx, normal, uv);

        tris_.push_back(idx);
        count++;

        if (d < 0)
        {
            if (count == 4)
            {
                // convert quad to triangle
                int x = tris_[tris_.size() - 2];
                int y = tris_[tris_.size() - 4];
                tris_.push_back(y);
                tris_.push_back(x);
            }
            count = 0;
        }

        i++;
    }
}

int FBXImporter::RemapVertIdx(int idx, const Vector& normal, const Vector2D& uvs)
{
    auto& seen = mapped_verts_.find(idx);
    if (seen != mapped_verts_.end())
    {
        const Vertex& v = verts_[idx];
        if (v.normal == normal && v.texCoords == uvs)
            return idx;

        // we have mapped this vert already, check if the noraml and uv are consistent
        for (int i : seen->second)
        {
            const Vertex& dupe = verts_[i];
            if (dupe.normal == normal && dupe.texCoords == uvs)
                return i;
        }

        // no existing match was found, create a new vertex
        idx = verts_.size();
        verts_.emplace_back(Vertex{v.positon, normal, uvs});
        seen->second.push_back(idx);
        return idx;
    }
    else
    {
        Vertex& v = verts_[idx];
        v.normal = normal;
        v.texCoords = uvs;

        mapped_verts_.emplace(idx, std::vector<int>{});
    }
    return idx;
}