#pragma once

#include <memory>
#include <vector>

#include "../Math/Elipsoid.h"
#include "Mesh.h"

class Primitives
{
public:
	static std::shared_ptr<Mesh> SubdivisionSurfacesElipsoid(const Elipsoid& elipsoid, int steps);

private:

	static void SubdivisionStep(std::vector<Vertex>& verts, std::vector<unsigned short>& outIndicies, unsigned short v0, unsigned short v1, unsigned short v2, int level);

};