#pragma once

#include "Math/Vector.h"
#include "Math/Math.h"

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

    // London
    const float mLatitude = 51.5f;
    const float mLongtitude = -0.1278f;
    // Reykjavik
    // const float mLatitude = 64.1f;
    // const float mLongtitude = -21.9426f;

    int mYear = 2021;
    int mOrdinalDate = 172;

    bool mPauseTime = false;
    float mCurrentTimeOfDay = 10.0f * 60.0f * 60.0f;
    int mTimeMultiplier = 100;
};