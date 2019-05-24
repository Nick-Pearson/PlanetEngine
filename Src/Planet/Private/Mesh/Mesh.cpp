#include "Mesh.h"

#include <iostream>

Mesh::Mesh(Vertex* verticies, size_t vertexCount, unsigned short* triangles, size_t trangleCount)
{
	mVerticies.assign(verticies, verticies + vertexCount);
	mTriangles.assign(triangles, triangles + trangleCount);

	RecalculateNormals();
}

#pragma optimize("", off)
void Mesh::RecalculateNormals()
{
	Vector s1{ 0.0f, 1.0f, 0.0f };
	Vector s2{ 1.0f, 0.0f, 0.0f };
	Vector r = s1.Cross(s2);
	

	// reset all normals
	for (Vertex& vert : mVerticies)
	{
		vert.normal = Vector{ 0.0f, 0.0f, 0.0f };
	}

	int numTris = mTriangles.size() / 3;
	for (int i = 0; i < numTris; ++i)
	{
		const int v1 = mTriangles[i * 3];
		const int v2 = mTriangles[(i * 3) + 1];
		const int v3 = mTriangles[(i * 3) + 2];

		Vector side1 = mVerticies[v2].positon - mVerticies[v1].positon;
		Vector side2 = mVerticies[v3].positon - mVerticies[v1].positon;

		Vector cross = side2.Cross(side1);
		cross.Normalise();

		mVerticies[v1].normal += cross;
		mVerticies[v2].normal += cross;
		mVerticies[v3].normal += cross;
	}

	// average all normals
	for (Vertex& vert : mVerticies)
	{
		vert.normal /= vert.normal.Length();
	}
}

#pragma optimize("", on)