#pragma once

#include "Math/Vector.h"
#include "Math/Math.h"
#include "SolarUtils.h"

struct FormattedTimeOfDay
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int seconds;
};

class TimeOfDay
{
 public:
    void Update(float deltaSeconds);

    Vector CalculateSunDirection() const;

    FormattedTimeOfDay FormatTimeOfDay() const;

 private:
    void AddTime(float seconds);

    bool override_sun_position_ = false;
    SunAngles sun_angles_;

    // London
    const float latitude_ = 51.5f;
    const float longtitude_ = -0.1278f;

    int year_ = 2021;
    int ordinal_date_ = 172;

    bool pause_time_ = false;
    float current_time_of_day_ = 10.0f * 60.0f * 60.0f;
    int time_multiplier_ = 100;
};