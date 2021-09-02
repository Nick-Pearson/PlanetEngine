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
 public:
    Mesh(const Vertex* verticies, size_t vertex_count, const uint16_t* triangles, size_t trangle_count);
    Mesh(const std::vector<Vertex>& verticies, const std::vector<uint16_t>& triangles);

    void RecalculateNormals();
    void FlipFaces();

    void Scale(const float& scaleFactor);
    void Scale(const Vector& scaleFactor);

    inline const Vertex* GetVertexData() const { return verticies_.data(); }
    inline size_t GetVertexCount() const { return verticies_.size(); }

    inline const uint16_t* GetTriangleData() const { return triangles_.data(); }
    inline size_t GetTriangleCount() const { return triangles_.size(); }

 private:
    std::vector<Vertex> verticies_;
    std::vector<uint16_t> triangles_;
};