#include "gtest/gtest.h"

#include "Math/Vector.h"

#define ASSERT_COMMUTATIVE(ASSERT_MACRO, PARAM_A, OP, PARAM_B) \
    ASSERT_MACRO(PARAM_A OP PARAM_B); \
    ASSERT_MACRO(PARAM_B OP PARAM_A);

TEST(VectorTest, vector_starts_zeroed)
{
    Vector vector{};

    EXPECT_TRUE(vector.IsZero());
    EXPECT_DOUBLE_EQ(0.0, vector.Length());
    EXPECT_DOUBLE_EQ(0.0, vector.LengthSqrd());
}

TEST(VectorTest, vector_with_initialisers_is_not_zero)
{
    Vector vector = Vector{0.1f, 0.0f, 0.0f};
    EXPECT_FALSE(vector.IsZero());

    vector = Vector{0.0f, 0.1f, 0.0f};
    EXPECT_FALSE(vector.IsZero());

    vector = Vector{0.0f, 0.0f, 0.1f};
    EXPECT_FALSE(vector.IsZero());
}

TEST(VectorTest, vector_equals_same_value_returns_true)
{
    Vector vectorA = Vector{425.0f, 75.0f, 20.0f};
    Vector vectorB = Vector{425.0f, 75.0f, 20.0f};

    ASSERT_COMMUTATIVE(EXPECT_TRUE, vectorA, ==, vectorB);
}


TEST(VectorTest, vector_equals_different_value_returns_true)
{
    Vector vectorA = Vector{425.0f, 75.0f, 20.0f};

    ASSERT_COMMUTATIVE(EXPECT_FALSE, vectorA, ==, Vector(854.0f, 75.0f, 20.0f));
    ASSERT_COMMUTATIVE(EXPECT_FALSE, vectorA, ==, Vector(425.0f, 66.0f, 20.0f));
    ASSERT_COMMUTATIVE(EXPECT_FALSE, vectorA, ==, Vector(425.0f, 75.0f, 45.0f));

    ASSERT_COMMUTATIVE(EXPECT_FALSE, vectorA, ==, Vector(854.0f, 66.0f, 20.0f));
    ASSERT_COMMUTATIVE(EXPECT_FALSE, vectorA, ==, Vector(854.0f, 75.0f, 45.0f));
    ASSERT_COMMUTATIVE(EXPECT_FALSE, vectorA, ==, Vector(425.0f, 66.0f, 45.0f));

    ASSERT_COMMUTATIVE(EXPECT_FALSE, vectorA, ==, Vector(854.0f, 66.0f, 45.0f));
}