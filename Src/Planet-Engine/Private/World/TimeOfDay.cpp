#include "TimeOfDay.h"

#include "Math/Vector.h"
#include "Math/Quaternion.h"

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
            31,
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
    if (!pause_time_)
    {
        AddTime(deltaSeconds * time_multiplier_);

        if (!override_sun_position_)
        {
            sun_angles_ = SolarUtils::CalculateSunAngles(ordinal_date_, current_time_of_day_ / SECONDS_IN_HOUR, latitude_, longtitude_);
        }
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
    ImGui::Checkbox("Pause Time", &pause_time_);
    ImGui::InputInt("Day of Year", &ordinal_date_);
    ImGui::InputInt("Time Multiplier", &time_multiplier_);
    ImGui::Checkbox("Override Sun", &override_sun_position_);
    ImGui::SliderAngle("Elevation", &sun_angles_.elevation);
    ImGui::SliderAngle("Azimuth", &sun_angles_.azimuth);
    Vector dir = CalculateSunDirection();
    ImGui::Text("Direction: %.2f %.2f %.2f (%.2f)", dir.x, dir.y, dir.z, dir.Length());
    ImGui::End();
}

Vector TimeOfDay::CalculateSunDirection() const
{
    // float altitude
    // Quaternion sunRotation{ Vector(Math::RadToDeg(mElevation), 0.0f, Math::RadToDeg(mAzimuth)) };
    float elvSin, elvCos, aziSin, aziCos;
    Math::SinAndCos(sun_angles_.elevation, &elvSin, &elvCos);
    Math::SinAndCos(sun_angles_.azimuth, &aziSin, &aziCos);

    // elevation vector rotated around the y axis by the azimuth
    // elevation vector = [0, sin E, -cos E]
    return Vector{
        -elvCos * aziSin,
        elvSin,
        -elvCos * aziCos};
}

FormattedTimeOfDay TimeOfDay::FormatTimeOfDay() const
{
    int wholeSeconds = static_cast<int>(current_time_of_day_);

    FormattedTimeOfDay result;
    result.year = year_;
    result.hour = (wholeSeconds / HOUR) % 24;
    result.minute = (wholeSeconds / MINUTE) % 60;
    result.seconds = wholeSeconds % 60;

    int day = ordinal_date_;
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
    current_time_of_day_ += seconds;
    if (current_time_of_day_ > SECONDS_IN_DAY)
    {
        current_time_of_day_ -= SECONDS_IN_DAY;
        ordinal_date_++;
        if (ordinal_date_ > 365)
        {
            ordinal_date_ -= 365;
            year_++;
        }
    }
}