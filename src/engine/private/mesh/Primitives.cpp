#include "Primitives.h"

#include <math.h>
#include <algorithm>
#include <unordered_map>

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

std::shared_ptr<Mesh> Primitives::SubdivisionSurfacesHemisphere(const Elipsoid& elipsoid, int steps)
{
    const float rootTwoOverThree = std::sqrt(2.0f) / 3.0f;
    const float negativeOneThird = -1.0f / 3.0f;
    const float rootSixOverThree = std::sqrt(6.0f) / 3.0f;

    std::vector<Vertex> verts;
    std::vector<uint16_t> indicies;

    // initialise 4 verts on the elipsoid
    verts.push_back(Vertex{ Vector{0.0f, 1.0f, 0.0f} });
    verts.push_back(Vertex{ Vector{0.0f, 0.0f, 2.0f * rootTwoOverThree} });
    verts.push_back(Vertex{ Vector{-rootSixOverThree, 0.0f, -rootTwoOverThree} });
    verts.push_back(Vertex{ Vector{rootSixOverThree, 0.0f, -rootTwoOverThree} });

    SubdivisionStep(&verts, &indicies, 0, 2, 1, steps);
    SubdivisionStep(&verts, &indicies, 0, 3, 2, steps);
    SubdivisionStep(&verts, &indicies, 0, 1, 3, steps);

    for (Vertex& vert : verts)
    {
        vert.normal = vert.positon;
        vert.positon *= Vector(elipsoid.sizeX, elipsoid.sizeY, elipsoid.sizeZ);
    }

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(verts, indicies);
    return mesh;
}

std::shared_ptr<Mesh> Primitives::IcoHemisphere(const Elipsoid& elipsoid, int order)
{
    const float rootFive = std::sqrt(5.0f);
    const float f = (1.0f + rootFive) / 2.0f;

    std::vector<Vertex> verts{10, Vertex{ Vector{ 0.0f, 0.0f, 0.0f } }};
    verts[0] = Vertex{ Vector{-1.0f, f, 0.0f} };
    verts[1] = Vertex{ Vector{1.0f, f, 0.0f} };
    verts[2] = Vertex{ Vector{0.0f, -1.0f, f} };
    verts[3] = Vertex{ Vector{0.0f, 1.0f, f} };
    verts[4] = Vertex{ Vector{0.0f, -1.0f, -f} };
    verts[5] = Vertex{ Vector{0.0f, 1.0f, -f } };
    verts[6] = Vertex{ Vector{f, 0.0f, -1.0f} };
    verts[7] = Vertex{ Vector{f, 0.0f, 1.0f} };
    verts[8] = Vertex{ Vector{-f, 0.0f, -1.0f} };
    verts[9] = Vertex{ Vector{-f, 0.0f, 1.0f} };

    uint16_t t[36] = {
        0, 9, 3,
        0, 3, 1,
        0, 1, 5,
        0, 5, 8,
        0, 8, 9,
        3, 9, 2,
        1, 3, 7,
        5, 1, 6,
        8, 5, 4,
        7, 6, 1,
        2, 7, 3,
        6, 4, 5
        };
    std::vector<uint16_t> indicies{t, t + (sizeof(t) / sizeof(uint16_t))};

    std::unordered_map<uint32_t, uint16_t> mid_cache{};

    auto addMidPoint = [&](uint16_t a, uint16_t b) {
        const uint32_t key = a | (b << 16);

        auto found = mid_cache.find(key);
        if (found == mid_cache.end())
        {
            auto v = static_cast<uint16_t>(verts.size());
            verts.push_back(Vertex{ (verts[a].positon + verts[b].positon) / 2.0f });

            mid_cache[key] = v;
            mid_cache[b | (a << 16)] = v;
            return v;
        }
        else
        {
            return found->second;
        }
    };


    std::vector<uint16_t> indicies_prev{indicies};
    for (int i = 0; i < order; i++)
    {
        // subdivide each triangle into 4 triangles
        indicies = std::vector<uint16_t>{indicies_prev.size() * 4, std::allocator<uint16_t>()};

        for (int k = 0; k < indicies_prev.size(); k += 3)
        {
            const auto v1 = indicies_prev[k + 0];
            const auto v2 = indicies_prev[k + 1];
            const auto v3 = indicies_prev[k + 2];
            const auto a = addMidPoint(v1, v2);
            const auto b = addMidPoint(v2, v3);
            const auto c = addMidPoint(v3, v1);

            auto t = k * 4;
            indicies[t++] = v1;
            indicies[t++] = a;
            indicies[t++] = c;

            indicies[t++] = v2;
            indicies[t++] = b;
            indicies[t++] = a;

            indicies[t++] = v3;
            indicies[t++] = c;
            indicies[t++] = b;

            indicies[t++] = a;
            indicies[t++] = b;
            indicies[t++] = c;
        }

        indicies_prev = std::vector{indicies};
    }

    for (Vertex& vert : verts)
    {
        vert.positon.Normalise();
        vert.normal = vert.positon;
        vert.positon *= Vector(elipsoid.sizeX, elipsoid.sizeY, elipsoid.sizeZ);
    }
    return std::make_shared<Mesh>(verts, indicies);
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

std::shared_ptr<Mesh> Primitives::Circle(float scale, int steps)
{
    std::vector<Vertex> verts{};
    verts.reserve(steps+1);
    verts.push_back(Vertex{ Vector{ 0.0f, 0.0f, 0.0f } });

    constexpr float tau = 6.283185307179586f;
    const float fraction = tau / steps;
    for (int i = 0; i < steps; ++i)
    {
        const float y = scale * std::cos(fraction * i);
        const float z = scale * std::sin(fraction * i);
        verts.push_back(Vertex{ Vector{ 0.0f, y, z } });
    }

    std::vector<uint16_t> indicies{};
    indicies.reserve(steps * 3);
    for (int i = 1; i < steps; ++i)
    {
        indicies.push_back(0);
        indicies.push_back(i);
        indicies.push_back(i+1);
    }
    indicies.push_back(0);
    indicies.push_back(steps);
    indicies.push_back(1);

    return std::make_shared<Mesh>(verts, indicies);
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
