#pragma once

#include <vector>

#include "GPUResourceManager.h"
#include "../Math/Vector.h"

struct Vertex
{
	Vertex(const Vector& inPos, const Vector& inNorm = Vector{ 1.0f, 0.0f, 0.0f }) :
		positon{inPos}, normal{inNorm}
	{}

	Vector positon;
	Vector normal;
};

class Mesh
{
	friend class GPUResourceManager;

public:
	Mesh(const Vertex* verticies, size_t vertexCount, const unsigned short* triangles, size_t trangleCount);
	Mesh(const std::vector<Vertex>& verticies, const std::vector<unsigned short>& triangles);

	inline bool IsLoaded() const { return handle != nullptr; }
	inline GPUMeshHandle* GetGPUHandle() const { return handle; }

	void RecalculateNormals();
	void FlipFaces();

private:
	std::vector<Vertex> mVerticies;
	std::vector<unsigned short> mTriangles;

	GPUMeshHandle* handle;
};