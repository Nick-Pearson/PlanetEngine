#include "Primitives.h"

std::shared_ptr<Mesh> Primitives::SubdivisionSurfacesElipsoid(const Elipsoid& elipsoid, int steps)
{
	const float rootTwoOverThree = std::sqrt(2.0f) / 3.0f;
	const float negativeOneThird = -1.0f / 3.0f;
	const float rootSixOverThree = std::sqrt(6.0f) / 3.0f;

	std::vector<Vertex> verts;
	std::vector<unsigned short> indicies;

	// initialise 4 verts on the elipsoid
	verts.push_back(Vertex{ Vector{0.0f, 0.0f, 1.0f} });
	verts.push_back(Vertex{ Vector{0.0f, 2.0f * rootTwoOverThree, negativeOneThird} });
	verts.push_back(Vertex{ Vector{-rootSixOverThree, -rootTwoOverThree, negativeOneThird} });
	verts.push_back(Vertex{ Vector{rootSixOverThree, -rootTwoOverThree, negativeOneThird} });

	SubdivisionStep(verts, indicies, 0, 1, 2, steps);
	SubdivisionStep(verts, indicies, 0, 2, 3, steps);
	SubdivisionStep(verts, indicies, 0, 3, 1, steps);
	SubdivisionStep(verts, indicies, 1, 3, 2, steps);

	for (Vertex& vert : verts)
	{
		vert.normal = -vert.positon;
		vert.positon *= Vector(elipsoid.sizeX, elipsoid.sizeY, elipsoid.sizeZ);
	}

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(verts, indicies);
	return mesh;
}

void Primitives::SubdivisionStep(std::vector<Vertex>& verts, std::vector<unsigned short>& outIndicies, unsigned short v0, unsigned short v1, unsigned short v2, int level)
{
	if (level > 0)
	{
		verts.push_back(Vertex{ (verts[v0].positon + verts[v1].positon) * 0.5f } );
		verts.push_back(Vertex{ (verts[v1].positon + verts[v2].positon) * 0.5f });
		verts.push_back(Vertex{ (verts[v2].positon + verts[v0].positon) * 0.5f });

		unsigned short v01 = (unsigned short)verts.size() - 3;
		unsigned short v12 = v01 + 1;
		unsigned short v20 = v12 + 1;

		verts[v01].positon.Normalise();
		verts[v12].positon.Normalise();
		verts[v20].positon.Normalise();

		--level;

		SubdivisionStep(verts, outIndicies, v0, v01, v20, level);
		SubdivisionStep(verts, outIndicies, v01, v1, v12, level);
		SubdivisionStep(verts, outIndicies, v01, v12, v20, level);
		SubdivisionStep(verts, outIndicies, v20, v12, v2, level);
	}
	else
	{
		outIndicies.push_back(v0);
		outIndicies.push_back(v1);
		outIndicies.push_back(v2);
	}
}
