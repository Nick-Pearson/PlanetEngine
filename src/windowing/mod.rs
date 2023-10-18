use crate::input::InputReader;

#[cfg(windows)]
pub mod winapi;

pub trait Window {
    fn show(&self);
    fn hide(&self);
    
    fn input(&self) -> &dyn InputReader;
}