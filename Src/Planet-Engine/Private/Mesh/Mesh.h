#pragma once

#include <vector>

#include "GPUResourceManager.h"
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
	Mesh(const Vertex* verticies, size_t vertexCount, const unsigned short* triangles, size_t trangleCount);
	Mesh(const std::vector<Vertex>& verticies, const std::vector<unsigned short>& triangles);

	void RecalculateNormals();
	void FlipFaces();

private:
	std::vector<Vertex> mVerticies;
	std::vector<unsigned short> mTriangles;
};