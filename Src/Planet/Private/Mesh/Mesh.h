#pragma once

#include <vector>

#include "MeshManager.h"

struct Vector
{
	float x;
	float y;
	float z;
};

struct Vertex
{
	Vertex(float x, float y, float z) :
		positon{x, y, z}
	{}

	Vector positon;
};

class Mesh
{
	friend class MeshManager;

public:
	Mesh(Vertex* verticies, size_t vertexCount, unsigned short* triangles, size_t trangleCount);

	inline bool IsLoaded() const { return handle != nullptr; }
	inline GPUMeshHandle* GetGPUHandle() const { return handle; }

private:
	std::vector<Vertex> mVerticies;
	std::vector<unsigned short> mTriangles;

	GPUMeshHandle* handle;
};