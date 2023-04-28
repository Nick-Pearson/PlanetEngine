#include "Mesh/Mesh.h"

#include <iostream>

Mesh::Mesh(const Vertex* verticies, size_t vertexCount, const uint16_t* triangles, size_t trangleCount)
{
    verticies_.assign(verticies, verticies + vertexCount);
    triangles_.assign(triangles, triangles + trangleCount);
}

Mesh::Mesh(const std::vector<Vertex>& verticies, const std::vector<uint16_t>& triangles)
{
    verticies_ = verticies;
    triangles_ = triangles;
}

void Mesh::RecalculateNormals()
{
    // reset all normals
    for (Vertex& vert : verticies_)
    {
        vert.normal = Vector{ 0.0f, 0.0f, 0.0f };
    }

    int numTris = triangles_.size() / 3;
    for (int i = 0; i < numTris; ++i)
    {
        const int v1 = triangles_[i * 3];
        const int v2 = triangles_[(i * 3) + 1];
        const int v3 = triangles_[(i * 3) + 2];

        Vector side1 = verticies_[v2].positon - verticies_[v1].positon;
        Vector side2 = verticies_[v3].positon - verticies_[v1].positon;

        Vector cross = side2.Cross(side1);
        cross.Normalise();

        verticies_[v1].normal += cross;
        verticies_[v2].normal += cross;
        verticies_[v3].normal += cross;
    }

    // average all normals
    for (Vertex& vert : verticies_)
    {
        vert.normal /= vert.normal.Length();
    }
}

void Mesh::FlipFaces()
{
    for (size_t i = 0; i < triangles_.size(); i += 3)
    {
        uint16_t tmp = triangles_[i];
        triangles_[i] = triangles_[i + 1];
        triangles_[i + 1] = tmp;
    }

    for (Vertex& v : verticies_)
    {
        v.normal *= -1.0f;
    }
}

void Mesh::Scale(const float& scaleFactor)
{
    for (Vertex& v : verticies_)
    {
        v.positon *= scaleFactor;
    }
}

void Mesh::Scale(const Vector& scaleFactor)
{
    for (Vertex& v : verticies_)
    {
        v.positon *= scaleFactor;
    }
}