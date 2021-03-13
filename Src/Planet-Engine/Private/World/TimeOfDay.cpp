#include "TimeOfDay.h"

#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "SolarUtils.h"

#include "imgui.h"

namespace
{
    static const char* MONTH_STRING[] = {
            "Jan",
            "Feb",
            "Mar",
            "Apr",
            "May",
            "Jun",
            "Jul",
            "Aug",
            "Sep",
            "Oct",
            "Nov",
            "Dec"
    };
    static const int DAYS_IN_MONTH[] = {
            31,
            28,
            31,
            30,
            30,
            31,
            31,
            30,
            31,
            30,
            31
    };

    static const int MINUTE = 60;
    static const int HOUR = MINUTE * 60;
    static const int DAY = HOUR * 24;
    static const float SECONDS_IN_HOUR = HOUR;
    static const float SECONDS_IN_DAY = DAY;
}  // namespace

void TimeOfDay::Update(float deltaSeconds)
{
    if (!mPauseTime)
    {
        AddTime(deltaSeconds * mTimeMultiplier);
    }

    ImGui::Begin("Time of Day");
    FormattedTimeOfDay formattedTime = FormatTimeOfDay();
    ImGui::Text("%d %s %d  %.2d:%.2d:%.2d UTC",
        formattedTime.day,
        MONTH_STRING[formattedTime.month],
        formattedTime.year,
        formattedTime.hour,
        formattedTime.minute,
        formattedTime.seconds);
    ImGui::Checkbox("Pause Time", &mPauseTime);
    ImGui::InputInt("Day of Year", &mOrdinalDate);
    ImGui::InputInt("Time Multiplier", &mTimeMultiplier);
    auto angles = SolarUtils::CalculateSunAngles(mOrdinalDate, mCurrentTimeOfDay / SECONDS_IN_HOUR, mLatitude, mLongtitude);
    ImGui::SliderAngle("Elevation", &angles.elevation);
    ImGui::SliderAngle("Azimuth", &angles.azimuth);
    Vector dir = CalculateSunDirection();
    ImGui::Text("Direction: %.2f %.2f %.2f (%.2f)", dir.x, dir.y, dir.z, dir.Length());
    ImGui::End();
}

Vector TimeOfDay::CalculateSunDirection() const
{
    auto angles = SolarUtils::CalculateSunAngles(mOrdinalDate, mCurrentTimeOfDay / SECONDS_IN_HOUR, mLatitude, mLongtitude);

    // float altitude
    // Quaternion sunRotation{ Vector(Math::RadToDeg(mElevation), 0.0f, Math::RadToDeg(mAzimuth)) };
    float elvSin, elvCos, aziSin, aziCos;
    Math::SinAndCos(angles.elevation, &elvSin, &elvCos);
    Math::SinAndCos(angles.azimuth, &aziSin, &aziCos);

    // elevation vector rotated around the y axis by the azimuth
    // elevation vector = [0, sin E, -cos E]
    return Vector{
        -elvCos * aziSin,
        elvSin,
        -elvCos * aziCos};
}

FormattedTimeOfDay TimeOfDay::FormatTimeOfDay() const
{
    int wholeSeconds = static_cast<int>(mCurrentTimeOfDay);

    FormattedTimeOfDay result;
    result.year = mYear;
    result.hour = (wholeSeconds / HOUR) % 24;
    result.minute = (wholeSeconds / MINUTE) % 60;
    result.seconds = wholeSeconds % 60;

    int day = mOrdinalDate;
    result.month = 0;
    while (day > DAYS_IN_MONTH[result.month])
    {
        day -= DAYS_IN_MONTH[result.month];
        result.month++;
    }
    result.day = day;
    return result;
}

void TimeOfDay::AddTime(float seconds)
{
    mCurrentTimeOfDay += seconds;
    if (mCurrentTimeOfDay > SECONDS_IN_DAY)
    {
        mCurrentTimeOfDay -= SECONDS_IN_DAY;
        mOrdinalDate++;
        if (mOrdinalDate > 365)
        {
            mOrdinalDate -= 365;
        }
    }
}