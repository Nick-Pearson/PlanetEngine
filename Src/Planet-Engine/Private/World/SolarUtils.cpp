#include "SolarUtils.h"

#include "Math/Math.h"

namespace SolarUtils
{
    float SolarUtils::CalculateDeclinationAngle(int dayOfYear)
    {
        return -23.45f * Math::Cos((PI * (2 * (dayOfYear + 10))) / 365.0f);
    }

    float SolarUtils::CalculateHourAngle(int dayOfYear, float currentTimeHours, float longitudeDegrees)
    {
        const float lstm_minutes = 15.0f * 0.0f;
        const float B = (2.0f * PI / 365.0f) * (dayOfYear - 81);
        const float eot_minutes = (9.87f * Math::Sin(2.0f * B)) - (7.53f * Math::Cos(B)) - (1.5f * Math::Sin(B));
        const float tc_minutes = 4.0f * (longitudeDegrees - lstm_minutes) + eot_minutes;
        const float lst_hours = currentTimeHours + (tc_minutes / 60.0f);
        return 15.0f * (lst_hours - 12.0f);
    }

    SunAngles SolarUtils::CalculateSunAngles(int dayOfYear, float currentTimeHours, float latitudeDegrees, float longitudeDegrees)
    {
        const float HRA = CalculateHourAngle(dayOfYear, currentTimeHours, longitudeDegrees);
        const float declinationAngle = CalculateDeclinationAngle(dayOfYear);

        float sinLatitude, cosLatitude, sinDeclination, cosDeclination;
        Math::SinAndCos(Math::DegToRad(latitudeDegrees), &sinLatitude, &cosLatitude);
        Math::SinAndCos(Math::DegToRad(declinationAngle), &sinDeclination, &cosDeclination);
        const float cosHRA = Math::Cos(Math::DegToRad(HRA));

        const float elevation = Math::ASin((sinDeclination * sinLatitude) + (cosDeclination * cosLatitude * cosHRA));
        const float cosAzimuth = ((sinDeclination * cosLatitude) - (cosDeclination * sinLatitude * cosHRA)) / Math::Cos(elevation);

        SunAngles angles;
        angles.elevation = elevation;
        angles.azimuth = Math::ACos(cosAzimuth);
        if (HRA > 0.0f)
        {
            angles.azimuth = (2.0f * PI) - angles.azimuth;
        }
        return angles;
    }
}  // namespace SolarUtils