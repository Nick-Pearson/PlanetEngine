#include "Primitives.h"

std::shared_ptr<Mesh> Primitives::SubdivisionSurfacesElipsoid(const Elipsoid& elipsoid, int steps)
{
    const float rootTwoOverThree = std::sqrt(2.0f) / 3.0f;
    const float negativeOneThird = -1.0f / 3.0f;
    const float rootSixOverThree = std::sqrt(6.0f) / 3.0f;

    std::vector<Vertex> verts;
    std::vector<uint16_t> indicies;

    // initialise 4 verts on the elipsoid
    verts.push_back(Vertex{ Vector{0.0f, 0.0f, 1.0f} });
    verts.push_back(Vertex{ Vector{0.0f, 2.0f * rootTwoOverThree, negativeOneThird} });
    verts.push_back(Vertex{ Vector{-rootSixOverThree, -rootTwoOverThree, negativeOneThird} });
    verts.push_back(Vertex{ Vector{rootSixOverThree, -rootTwoOverThree, negativeOneThird} });

    SubdivisionStep(&verts, &indicies, 0, 1, 2, steps);
    SubdivisionStep(&verts, &indicies, 0, 2, 3, steps);
    SubdivisionStep(&verts, &indicies, 0, 3, 1, steps);
    SubdivisionStep(&verts, &indicies, 1, 3, 2, steps);

    for (Vertex& vert : verts)
    {
        vert.normal = vert.positon;
        vert.positon *= Vector(elipsoid.sizeX, elipsoid.sizeY, elipsoid.sizeZ);
    }

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(verts, indicies);
    return mesh;
}

std::shared_ptr<Mesh> Primitives::Cube(float scale)
{
    Vertex v[] = {
        Vertex{ Vector{ -1.0f, -1.0f, 1.0f },  Vector{}, Vector2D{ 0.0f, 1.0f } },
        Vertex{ Vector{ 1.0f,  -1.0f, 1.0f },  Vector{}, Vector2D{ 1.0f, 1.0f } },
        Vertex{ Vector{ -1.0f, -1.0f, -1.0f }, Vector{}, Vector2D{ 0.0f, 0.0f } },
        Vertex{ Vector{ 1.0f,  -1.0f, -1.0f }, Vector{}, Vector2D{ 1.0f, 0.0f } },
        Vertex{ Vector{ -1.0f, 1.0f, 1.0f },   Vector{}, Vector2D{ 0.0f, 1.0f } },
        Vertex{ Vector{ 1.0f,  1.0f, 1.0f },   Vector{}, Vector2D{ 1.0f, 1.0f } },
        Vertex{ Vector{ -1.0f, 1.0f, -1.0f },  Vector{}, Vector2D{ 0.0f, 0.0f } },
        Vertex{ Vector{ 1.0f,  1.0f, -1.0f},   Vector{}, Vector2D{ 1.0f, 0.0f }}
    };
    uint16_t t[] = {
        // front
        0, 1, 5,
        0, 5, 4,

        // back 2,3,6,7
        3, 2, 6,
        3, 6, 7,

        // top
        6, 4, 5,
        6, 5, 7,

        // bottom 0,1,2,3
        0, 3, 1,
        0, 2, 3,

        // right
        5, 1, 3,
        5, 3, 7,

        // left 6,4,2,0
        0, 6, 2,
        0, 4, 6
    };
    std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(v, 8, t, 36);
    cube->RecalculateNormals();
    cube->Scale(Vector{ scale, scale, scale });
    return cube;
}

std::shared_ptr<Mesh> Primitives::Plane(float scale)
{
    Vertex v[] = {
        Vertex{ Vector{ scale *  1.0f, scale * -1.0f, 0.0f }, Vector{ 0.0f, 0.0f, 1.0f }, Vector2D{ 1.0f, 0.0f } },
        Vertex{ Vector{ scale * -1.0f, scale *  1.0f, 0.0f }, Vector{ 0.0f, 0.0f, 1.0f }, Vector2D{ 0.0f, 1.0f } },
        Vertex{ Vector{ scale *  1.0f, scale *  1.0f, 0.0f }, Vector{ 0.0f, 0.0f, 1.0f }, Vector2D{ 1.0f, 1.0f } },
        Vertex{ Vector{ scale * -1.0f, scale * -1.0f, 0.0f }, Vector{ 0.0f, 0.0f, 1.0f }, Vector2D{ 0.0f, 0.0f } },
    };
    uint16_t t[] = {
        0, 1, 2,
        0, 3, 1,
    };
    return std::make_shared<Mesh>(v, 4, t, 6);
}

void Primitives::SubdivisionStep(std::vector<Vertex>* verts, std::vector<uint16_t>* outIndicies, uint16_t v0, uint16_t v1, uint16_t v2, int level)
{
    if (level > 0)
    {
        verts->push_back(Vertex{ ((*verts)[v0].positon + (*verts)[v1].positon) * 0.5f });
        verts->push_back(Vertex{ ((*verts)[v1].positon + (*verts)[v2].positon) * 0.5f });
        verts->push_back(Vertex{ ((*verts)[v2].positon + (*verts)[v0].positon) * 0.5f });

        uint16_t v01 = (uint16_t)verts->size() - 3;
        uint16_t v12 = v01 + 1;
        uint16_t v20 = v12 + 1;

        (*verts)[v01].positon.Normalise();
        (*verts)[v12].positon.Normalise();
        (*verts)[v20].positon.Normalise();

        --level;

        SubdivisionStep(verts, outIndicies, v0, v01, v20, level);
        SubdivisionStep(verts, outIndicies, v01, v1, v12, level);
        SubdivisionStep(verts, outIndicies, v01, v12, v20, level);
        SubdivisionStep(verts, outIndicies, v20, v12, v2, level);
    }
    else
    {
        outIndicies->push_back(v0);
        outIndicies->push_back(v1);
        outIndicies->push_back(v2);
    }
}
