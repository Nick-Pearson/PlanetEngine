#pragma once

struct SunAngles
{
    // in radians
    float elevation;
    float azimuth;
};

namespace SolarUtils
{
    extern float CalculateDeclinationAngle(int dayOfYear);

    extern float CalculateHourAngle(int dayOfYear, float currentTimeHours, float longitudeDegrees);

    SunAngles CalculateSunAngles(int dayOfYear, float currentTimeHours, float latitudeDegrees, float longitudeDegrees);
}  // namespace SolarUtils
