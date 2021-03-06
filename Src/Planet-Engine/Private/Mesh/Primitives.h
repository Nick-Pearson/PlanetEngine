#pragma once

#include <memory>
#include <vector>

#include "Math/Elipsoid.h"
#include "Mesh/Mesh.h"

class Primitives
{
 public:
    static std::shared_ptr<Mesh> SubdivisionSurfacesElipsoid(const Elipsoid& elipsoid, int steps);

    static std::shared_ptr<Mesh> Cube(float scale);

    static std::shared_ptr<Mesh> Plane(float scale);

 private:
    static void SubdivisionStep(std::vector<Vertex>* verts, std::vector<uint16_t>* outIndicies, uint16_t v0, uint16_t v1, uint16_t v2, int level);
};