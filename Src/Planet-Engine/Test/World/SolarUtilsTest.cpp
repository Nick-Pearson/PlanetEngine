#include "gtest/gtest.h"

#include "World/SolarUtils.h"
#include "Math/Vector.h"
#include "Math/Math.h"

const float epsilon = 1e-03;

TEST(SolarUtilsTest, declination_spring_equinox)
{
  float angle = SolarUtils::CalculateDeclinationAngle(81);

  EXPECT_NEAR(-0.1f, angle, epsilon);
}

TEST(SolarUtilsTest, declination_summer_solstice)
{
  float angle = SolarUtils::CalculateDeclinationAngle(172);

  EXPECT_NEAR(23.45f, angle, epsilon);
}

TEST(SolarUtilsTest, declination_autumn_equinox)
{
  float angle = SolarUtils::CalculateDeclinationAngle(264);

  EXPECT_NEAR(-0.1f, angle, epsilon);
}

TEST(SolarUtilsTest, declination_winter_solstice)
{
  float angle = SolarUtils::CalculateDeclinationAngle(355);

  EXPECT_NEAR(-23.45f, angle, epsilon);
}

TEST(SolarUtilsTest, hour_angle_sydney_summer_soltice)
{
  float angle = SolarUtils::CalculateHourAngle(172, 2.0f, 150.0f);

  EXPECT_NEAR(187.14f, angle, epsilon);
}

TEST(SolarUtilsTest, hour_angle_midday_sydney_summer_soltice)
{
  float angle = SolarUtils::CalculateHourAngle(172, 2.0f, 150.0f);

  EXPECT_NEAR(179.64f, angle, epsilon);
}

TEST(SolarUtilsTest, sun_angles_midday_london_summer_soltice)
{
  auto angles = SolarUtils::CalculateSunAngles(172, 12.0f, 51.5f, -0.1278f);

  EXPECT_NEAR(61.95f, Math::RadToDeg(angles.elevation), 1e-1);
  EXPECT_NEAR(179.04f, Math::RadToDeg(angles.azimuth), 1e-1);
}