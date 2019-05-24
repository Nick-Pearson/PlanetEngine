#include "Mesh.h"

Mesh::Mesh(Vertex* verticies, size_t vertexCount, unsigned short* triangles, size_t trangleCount)
{
	mVerticies.assign(verticies, verticies + vertexCount);
	mTriangles.assign(triangles, triangles + trangleCount);
}
