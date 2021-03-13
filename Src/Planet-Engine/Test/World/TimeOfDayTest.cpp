#include "gtest/gtest.h"

#include "World/TimeOfDay.h"
#include "Math/Vector.h"
#include "Math/Math.h"

const float epsilon = 1e-06;

// TEST(TimeOfDayTest, sun_due_north)
// {
//   Vector dir = TimeOfDay::CalculateSunDirection(0.0f, 0.0f);

//   EXPECT_NEAR(0.0f, dir.x, epsilon);
//   EXPECT_NEAR(0.0f, dir.y, epsilon);
//   EXPECT_NEAR(-1.0f, dir.z, epsilon);
// }

// TEST(TimeOfDayTest, sun_due_south)
// {
//   Vector dir = TimeOfDay::CalculateSunDirection(0.0f, PI);

//   EXPECT_NEAR(0.0f, dir.x, epsilon);
//   EXPECT_NEAR(0.0f, dir.y, epsilon);
//   EXPECT_NEAR(1.0f, dir.z, epsilon);
// }

// TEST(TimeOfDayTest, sun_due_east)
// {
//   Vector dir = TimeOfDay::CalculateSunDirection(0.0f, 0.5f * PI);

//   EXPECT_NEAR(-1.0f, dir.x, epsilon);
//   EXPECT_NEAR(0.0f, dir.y, epsilon);
//   EXPECT_NEAR(0.0f, dir.z, epsilon);
// }

// TEST(TimeOfDayTest, sun_due_west)
// {
//   Vector dir = TimeOfDay::CalculateSunDirection(0.0f, 1.5f * PI);

//   EXPECT_NEAR(1.0f, dir.x, epsilon);
//   EXPECT_NEAR(0.0f, dir.y, epsilon);
//   EXPECT_NEAR(0.0f, dir.z, epsilon);
// }

// TEST(TimeOfDayTest, sun_zenith)
// {
//   Vector dir = TimeOfDay::CalculateSunDirection(0.5f * PI, 0.0f);

//   EXPECT_NEAR(0.0f, dir.x, epsilon);
//   EXPECT_NEAR(1.0f, dir.y, epsilon);
//   EXPECT_NEAR(0.0f, dir.z, epsilon);
// }

// TEST(TimeOfDayTest, sun_anti_zenith)
// {
//   Vector dir = TimeOfDay::CalculateSunDirection(1.5f * PI, 0.0f);

//   EXPECT_NEAR(0.0f, dir.x, epsilon);
//   EXPECT_NEAR(-1.0f, dir.y, epsilon);
//   EXPECT_NEAR(0.0f, dir.z, epsilon);
// }