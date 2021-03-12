#pragma once

#include <vector>

#include "Math/Vector.h"
#include "Math/Vector2D.h"

struct Vertex
{
    Vertex(const Vector& inPos, const Vector& inNorm = Vector{ 1.0f, 0.0f, 0.0f }, const Vector2D& inTexCoords = Vector2D{ 0.0f, 0.0f }) :
        positon{ inPos }, normal{ inNorm }, texCoords(inTexCoords)
    {}

    Vector positon;
    Vector normal;
    Vector2D texCoords;
};

class Mesh
{
    friend class GPUResourceManager;

 public:
    Mesh(const Vertex* verticies, size_t vertexCount, const uint16_t* triangles, size_t trangleCount);
    Mesh(const std::vector<Vertex>& verticies, const std::vector<uint16_t>& triangles);

    void RecalculateNormals();
    void FlipFaces();

    void Scale(const Vector& scaleFactor);

 private:
    std::vector<Vertex> mVerticies;
    std::vector<uint16_t> mTriangles;
};