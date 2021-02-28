#include "OBJImporter.h"
#include "Mesh.h"

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

namespace
{
    std::vector<std::string> split(const std::string& str, const char delim)
    {
        std::string p;
        std::vector<std::string> out;

        for (const char c : str)
        {
            if (c == delim)
            {
                out.push_back(p);
                p = "";
            }
            else
            {
                p += c;
            }
        }

        out.push_back(p);
        return out;
    }
}  // namespace

std::shared_ptr<Mesh> OBJImporter::Import(const char* filepath, float scaleFactor /*= 1.0f*/)
{
    std::ifstream file;
    file.open(filepath, std::ios::in);

    if (!file.is_open())
    {
        return nullptr;
    }

    std::vector<Vertex> verts;
    std::vector<uint16_t> tris;
    std::unordered_map<std::string, int> vertMap;

    std::vector<Vector> vertPositions;
    std::vector<Vector> vertNormals;

    auto getVertexIdx = [&](const std::string& key) {
        auto it = vertMap.find(key);
        if (it != vertMap.end())
        {
            return (uint16_t)it->second;
        }

        std::vector<std::string> parts = split(key, '/');

        int posIdx = std::atoi(parts[0].c_str());
        Vertex v{ vertPositions[posIdx - 1] };

        if (parts.size() > 1 && parts[1] != "")
        {
            // TODO: Texture coords
        }

        if (parts.size() > 2 && parts[2] != "")
        {
            // normals
            int normalIdx = std::atoi(parts[2].c_str());
            v.normal = vertNormals[normalIdx - 1];
        }

        uint16_t idx = (uint16_t)verts.size();
        verts.push_back(v);
        vertMap.emplace(key, idx);

        return idx;
    };

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> parts = split(line, ' ');

        if (parts[0] == "#")
        {
            // comment
            continue;
        }
        else if (parts[0] == "v" && parts.size() == 4)
        {
            // vertex
            float x = static_cast<float>(std::atof(parts[1].c_str()));
            float y = static_cast<float>(std::atof(parts[2].c_str()));
            float z = static_cast<float>(std::atof(parts[3].c_str()));

            vertPositions.push_back(Vector{x, y, z} * scaleFactor);
        }
        else if (parts[0] == "vn" && parts.size() == 4)
        {
            // normal
            Vector normal;
            normal.x = static_cast<float>(std::atof(parts[1].c_str()));
            normal.y = static_cast<float>(std::atof(parts[2].c_str()));
            normal.z = static_cast<float>(std::atof(parts[3].c_str()));

            normal.Normalise();
            vertNormals.push_back(normal);
        }
        else if (parts[0] == "f" && parts.size() > 3)
        {
            // face
            uint16_t x = getVertexIdx(parts[1]);
            uint16_t y = getVertexIdx(parts[2]);
            uint16_t z = getVertexIdx(parts[3]);

            tris.push_back(x);
            tris.push_back(y);
            tris.push_back(z);

            if (parts.size() > 4)
            {
                uint16_t w = getVertexIdx(parts[4]);

                tris.push_back(z);
                tris.push_back(w);
                tris.push_back(x);
            }
        }
        else
        {
            // error invalid obj directive
            // LOG_WARN(AssetImport, "Invalid line in OBJ file\"%s\"", line.c_str());
        }
    }

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(verts, tris);

    if (vertNormals.size() == 0)
    {
        mesh->RecalculateNormals();
    }

    // TODO:
    // mesh->ValidateMesh();

    return mesh;
}

