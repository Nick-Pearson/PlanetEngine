#include "gtest/gtest.h"

#include "PlanetTest.h"
#include "Math/Vector.h"
#include "Math/Vector2D.h"
#include "Mesh/FBXImporter.h"
#include "Mesh/Mesh.h"

TEST(FBXImporterTest, import_plane_vertex_positions)
{
    std::shared_ptr<Mesh> mesh = FBXImporter::Import("AssetsTest/plane.fbx");

    EXPECT_TRUE(mesh.get() != nullptr);
    ASSERT_EQ(mesh->GetVertexCount(), 4);
    EXPECT_EQ(mesh->GetVertexData()[0].positon, Vector(-1.0f, -1.0f, 0.0f));
    EXPECT_EQ(mesh->GetVertexData()[1].positon, Vector(1.0f, -1.0f, 0.0f));
    EXPECT_EQ(mesh->GetVertexData()[2].positon, Vector(-1.0f, 1.0f, 0.0f));
    EXPECT_EQ(mesh->GetVertexData()[3].positon, Vector(1.0f, 1.0f, 0.0f));
}

TEST(FBXImporterTest, import_vertex_normals)
{
    std::shared_ptr<Mesh> mesh = FBXImporter::Import("AssetsTest/plane.fbx");

    EXPECT_TRUE(mesh.get() != nullptr);
    ASSERT_EQ(mesh->GetVertexCount(), 4);
    EXPECT_EQ(mesh->GetVertexData()[0].normal, Vector(0.0f, 0.0f, 1.0f));
    EXPECT_EQ(mesh->GetVertexData()[1].normal, Vector(0.0f, 0.0f, 1.0f));
    EXPECT_EQ(mesh->GetVertexData()[2].normal, Vector(0.0f, 0.0f, 1.0f));
    EXPECT_EQ(mesh->GetVertexData()[3].normal, Vector(0.0f, 0.0f, 1.0f));
}

TEST(FBXImporterTest, import_vertex_uvs)
{
    std::shared_ptr<Mesh> mesh = FBXImporter::Import("AssetsTest/plane.fbx");

    EXPECT_TRUE(mesh.get() != nullptr);
    EXPECT_EQ(mesh->GetVertexData()[0].texCoords, Vector2D(1.0f, 1.0f));
    EXPECT_EQ(mesh->GetVertexData()[1].texCoords, Vector2D(0.0f, 1.0f));
    EXPECT_EQ(mesh->GetVertexData()[2].texCoords, Vector2D(1.0f, 0.0f));
    EXPECT_EQ(mesh->GetVertexData()[3].texCoords, Vector2D(0.0f, 0.0f));
}

TEST(FBXImporterTest, import_plane_triangles)
{
    std::shared_ptr<Mesh> mesh = FBXImporter::Import("AssetsTest/plane.fbx");

    EXPECT_TRUE(mesh.get() != nullptr);
}