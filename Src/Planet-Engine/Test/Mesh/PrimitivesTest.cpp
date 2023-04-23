#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Mesh/Primitives.h"

using ::testing::UnorderedElementsAreArray;
using ::testing::Field;

TEST(PrimitivesTest, level_one_ico_hemisphere)
{
    auto mesh = Primitives::IcoHemisphere(Elipsoid(100.0f), 1);

    ASSERT_EQ(mesh->GetVertexCount(), 31);
    ASSERT_EQ(mesh->GetTriangleCount(), 144);
}
