use chrono::{Datelike, NaiveDate, NaiveDateTime, Timelike};
use glam::Vec3;
use log::info;

use std::{f32::consts::TAU, time::Duration};

#[derive(Default)]
pub struct GeographicCoord {
    latitude: f32,
    longtitude: f32,
}

impl GeographicCoord {
    pub const LONDON: Self = GeographicCoord::new(51.500_73, -0.124625);

    pub const fn new(latitude: f32, longtitude: f32) -> GeographicCoord {
        GeographicCoord {
            latitude,
            longtitude,
        }
    }

    // returns the latitude (in degrees)
    pub fn latitude(&self) -> f32 {
        self.latitude
    }

    // returns the longtitude (in degrees)
    pub fn longtitude(&self) -> f32 {
        self.longtitude
    }
}

#[derive(Default)]
pub struct HorizontalCoord {
    elevation: f32,
    azimuth: f32,
}

impl HorizontalCoord {
    pub fn new(elevation: f32, azimuth: f32) -> HorizontalCoord {
        HorizontalCoord { elevation, azimuth }
    }

    // returns the elevation angle (in radians)
    pub fn elevation(&self) -> f32 {
        self.elevation
    }

    // returns the azimuth angle (in radians)
    pub fn azimuth(&self) -> f32 {
        self.azimuth
    }
}

pub struct TimeOfDay {
    location: GeographicCoord,
    sun_angles: HorizontalCoord,

    current_time_of_day: NaiveDateTime,
    time_multiplier: f32,
}

impl TimeOfDay {
    const MINUTE: i32 = 60;
    const HOUR: i32 = Self::MINUTE * 60;
    const DAY: i32 = Self::HOUR * 24;
    const SECONDS_IN_HOUR: f32 = Self::HOUR as f32;
    const SECONDS_IN_DAY: f32 = Self::DAY as f32;

    pub fn new(location: GeographicCoord) -> TimeOfDay {
        TimeOfDay {
            location,
            sun_angles: HorizontalCoord::default(),
            current_time_of_day: NaiveDate::from_ymd_opt(2021, 7, 1)
                .unwrap()
                .and_hms_opt(10, 00, 00)
                .unwrap(),
            time_multiplier: 1000.0,
        }
    }

    pub fn update(&mut self, delta_time: f32) {
        self.add_time(delta_time * self.time_multiplier);

        self.sun_angles = Self::calculate_sun_angles(
            self.current_time_of_day.ordinal(),
            self.hour_f32(),
            &self.location,
        );

        info!("current time: {}", self.current_time_of_day);
    }

    fn hour_f32(&self) -> f32 {
        self.current_time_of_day.hour() as f32
            + (self.current_time_of_day.minute() as f32 / 60.0)
            + (self.current_time_of_day.second() as f32 / 3600.0)
    }

    pub fn add_time(&mut self, seconds: f32) {
        self.current_time_of_day += Duration::from_secs_f32(seconds);
    }

    pub fn calculate_sun_direction(&self) -> Vec3 {
        let elv_sin = self.sun_angles.elevation().sin();
        let elv_cos = self.sun_angles.elevation().cos();
        let azi_sin = self.sun_angles.azimuth().sin();
        let azi_cos = self.sun_angles.azimuth().cos();

        // Elevation vector rotated around the y-axis by the azimuth
        // Elevation vector = [0, sin E, -cos E]
        Vec3::new(-elv_cos * azi_sin, elv_sin, -elv_cos * azi_cos)
    }

    fn calculate_declination_angle(day_of_year: u32) -> f32 {
        -23.45 * ((TAU * (day_of_year + 10) as f32) / 365.0).cos()
    }

    fn calculate_hour_angle(
        day_of_year: u32,
        current_time_hours: f32,
        longitude_degrees: f32,
    ) -> f32 {
        let lstm_minutes = 15.0_f32 * 0.0_f32;
        let b = (TAU / 365.0_f32) * (day_of_year - 81) as f32;
        let eot_minutes = (9.87 * (2.0 * b).sin()) - (7.5 * b.cos()) - (1.5 * b.sin());
        let tc_minutes = 4.0 * (longitude_degrees - lstm_minutes) + eot_minutes;
        let lst_hours = current_time_hours + (tc_minutes / 60.0);
        15.0 * (lst_hours - 12.0)
    }

    fn calculate_sun_angles(
        day_of_year: u32,
        current_time_hours: f32,
        location: &GeographicCoord,
    ) -> HorizontalCoord {
        let hra =
            Self::calculate_hour_angle(day_of_year, current_time_hours, location.longtitude());
        let declination_angle = Self::calculate_declination_angle(day_of_year);

        let latitude_radians = f32::to_radians(location.latitude());
        let declination_radians = f32::to_radians(declination_angle);

        let sin_latitude = latitude_radians.sin();
        let cos_latitude = latitude_radians.cos();
        let sin_declination = declination_radians.sin();
        let cos_declination = declination_radians.cos();
        let cos_hra = hra.to_radians().cos();

        let elevation =
            ((sin_declination * sin_latitude) + (cos_declination * cos_latitude * cos_hra)).asin();
        let cos_azimuth = ((sin_declination * cos_latitude)
            - (cos_declination * sin_latitude * cos_hra))
            / elevation.cos();

        let mut azimuth = cos_azimuth.acos();
        if hra > 0.0 {
            azimuth = TAU - azimuth;
        }

        HorizontalCoord::new(elevation, azimuth)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use assert_approx_eq::assert_approx_eq;

    #[test]
    fn test_declination_spring_equinox() {
        let angle = TimeOfDay::calculate_declination_angle(81);
        assert_approx_eq!(-0.1, angle, 1e-3);
    }

    #[test]
    fn test_declination_summer_soltice() {
        let angle = TimeOfDay::calculate_declination_angle(172);
        assert_approx_eq!(23.45, angle, 1e-3);
    }

    #[test]
    fn test_declination_autumn_equinox() {
        let angle = TimeOfDay::calculate_declination_angle(264);
        assert_approx_eq!(-0.1, angle, 1e-3);
    }

    #[test]
    fn test_declination_winter_soltice() {
        let angle = TimeOfDay::calculate_declination_angle(355);
        assert_approx_eq!(-23.45, angle, 1e-3);
    }

    #[test]
    fn test_hour_angle_midday_sydney_summer_soltice() {
        let angle = TimeOfDay::calculate_hour_angle(172, 2.0, 150.0);
        assert_approx_eq!(-0.36, angle, 1e-1);
    }

    #[test]
    fn test_sun_angles_midday_london_summer_soltice() {
        let angles = TimeOfDay::calculate_sun_angles(172, 12.0, &GeographicCoord::LONDON);

        assert_approx_eq!(61.95, angles.elevation.to_degrees(), 1e-1);
        assert_approx_eq!(179.04, angles.azimuth.to_degrees(), 1e-1);
    }
}
