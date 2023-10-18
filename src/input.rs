#[derive(Debug, Hash, Eq, PartialEq)]
pub enum KeyCode {
    Character(char),

    LeftMouseButton,
    RightMouseButton,
    MiddleMouseButton,
    Backspace,
    Return,
    Tab,
    LeftShift,
    RightShift,
    LeftCtrl,
    RightCtrl,
    Alt,
    Pause,
    CapsLock,
    Esc,
    Space,
    PageUp,
    PageDown,
    End,
    Home,
    LeftArrow,
    UpArrow,
    RightArrow,
    DownArrow,
    Insert,
    Delete,
    LeftSuper,
    RightSuper,
}

pub trait InputReader {
    fn is_key_down(&self, key_code: KeyCode) -> bool;
}
