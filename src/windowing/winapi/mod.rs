use crate::input::KeyCode;
use windows::Win32::UI::Input::KeyboardAndMouse::*;
use windows::{
    core::*, Win32::Foundation::*, Win32::System::LibraryLoader::*,
    Win32::UI::WindowsAndMessaging::*,
};

use std::collections::HashSet;
use std::ffi::c_void;
use std::mem::transmute;

impl KeyCode {
    pub fn from_virtual_key_code(virtual_key_code: usize) -> std::result::Result<KeyCode, String> {
        let maybe_char: std::result::Result<u8, _> = virtual_key_code.try_into();
        if maybe_char.is_ok() {
            let c = maybe_char.unwrap() as char;
            if c.is_alphanumeric() {
                return Ok(KeyCode::Character(c));
            }
        }

        match VIRTUAL_KEY(virtual_key_code as u16) {
            VK_LBUTTON => Ok(KeyCode::LeftMouseButton),
            VK_RBUTTON => Ok(KeyCode::RightMouseButton),
            VK_MBUTTON => Ok(KeyCode::MiddleMouseButton),
            VK_BACK => Ok(KeyCode::Backspace),
            VK_TAB => Ok(KeyCode::Tab),
            VK_RETURN => Ok(KeyCode::Return),
            VK_LSHIFT => Ok(KeyCode::LeftShift),
            VK_SHIFT => Ok(KeyCode::LeftShift),
            VK_RSHIFT => Ok(KeyCode::RightShift),
            VK_LCONTROL => Ok(KeyCode::LeftCtrl),
            VK_CONTROL => Ok(KeyCode::LeftCtrl),
            VK_RCONTROL => Ok(KeyCode::RightCtrl),
            VK_MENU => Ok(KeyCode::Alt),
            VK_PAUSE => Ok(KeyCode::Pause),
            VK_CAPITAL => Ok(KeyCode::CapsLock),
            VK_ESCAPE => Ok(KeyCode::Esc),
            VK_SPACE => Ok(KeyCode::Space),
            VK_PRIOR => Ok(KeyCode::PageUp),
            VK_NEXT => Ok(KeyCode::PageDown),
            VK_END => Ok(KeyCode::End),
            VK_HOME => Ok(KeyCode::Home),
            VK_LEFT => Ok(KeyCode::LeftArrow),
            VK_UP => Ok(KeyCode::UpArrow),
            VK_RIGHT => Ok(KeyCode::RightArrow),
            VK_DOWN => Ok(KeyCode::DownArrow),
            VK_INSERT => Ok(KeyCode::Insert),
            VK_DELETE => Ok(KeyCode::Delete),
            VK_LWIN => Ok(KeyCode::LeftSuper),
            VK_RWIN => Ok(KeyCode::RightSuper),
            _ => Err("Unrecognised key code".into()),
        }
    }
}

trait WinProc {
    fn wndproc(&mut self, window: HWND, message: u32, wparam: WPARAM, lparam: LPARAM) -> bool;
}

struct InputWinProc {
    keys_pressed: HashSet<KeyCode>,
    mouse_position: (i32, i32),
    mouse_delta: (i32, i32),
}

impl InputWinProc {
    pub fn new() -> InputWinProc {
        InputWinProc {
            keys_pressed: HashSet::new(),
            mouse_position: (-1, -1),
            mouse_delta: (0, 0),
        }
    }
}

impl WinProc for InputWinProc {
    fn wndproc(&mut self, _window: HWND, message: u32, wparam: WPARAM, lparam: LPARAM) -> bool {
        match message {
            WM_KEYDOWN | WM_SYSKEYDOWN => {
                if lparam.0 & 0x40000000 != 0 {
                    return true;
                }

                let key = KeyCode::from_virtual_key_code(wparam.0);
                match key {
                    Ok(k) => {
                        self.keys_pressed.insert(k);
                        true
                    }
                    Err(_) => false,
                }
            }
            WM_KEYUP | WM_SYSKEYUP => {
                let key = KeyCode::from_virtual_key_code(wparam.0);
                match key {
                    Ok(k) => {
                        self.keys_pressed.remove(&k);
                        true
                    }
                    Err(_) => false,
                }
            }
            WM_MOUSEMOVE => {
                let (mouse_x, mouse_y) = (lparam.0 as i32, (lparam.0 >> 32) as i32);

                if self.mouse_position.0 != -1 {
                    self.mouse_delta = (
                        mouse_x - self.mouse_position.0,
                        mouse_y - self.mouse_position.1,
                    );
                }

                self.mouse_position = (mouse_x, mouse_y);
                true
            }
            _ => false,
        }
    }
}

struct MultiplexWinProc {
    handlers: Vec<Box<dyn WinProc>>,
}

impl MultiplexWinProc {
    fn new() -> MultiplexWinProc {
        MultiplexWinProc {
            handlers: Vec::new(),
        }
    }

    pub fn add_handler(&mut self, handler: Box<dyn WinProc>) {
        self.handlers.push(handler);
    }
}

impl WinProc for MultiplexWinProc {
    fn wndproc(&mut self, window: HWND, message: u32, wparam: WPARAM, lparam: LPARAM) -> bool {
        for handler in self.handlers.iter_mut() {
            let result = handler.as_mut().wndproc(window, message, wparam, lparam);
            if result {
                return true;
            }
        }
        false
    }
}

pub struct WinAPIWindow {
    pub hwnd: HWND,
    handler_ptr: *mut MultiplexWinProc,
}

impl WinAPIWindow {
    pub fn new(size_x: i32, size_y: i32) -> Result<WinAPIWindow> {
        let instance = unsafe { GetModuleHandleA(None)? };

        let input_handler = Box::new(InputWinProc::new());

        let mut handler = Box::new(MultiplexWinProc::new());
        handler.add_handler(input_handler);
        let handler_ptr = Box::into_raw(handler);

        let wc: WNDCLASSEXA = WNDCLASSEXA {
            cbSize: std::mem::size_of::<WNDCLASSEXA>() as u32,
            style: CS_OWNDC,
            lpfnWndProc: Some(wndproc_bootstrap),
            hInstance: instance,
            lpszClassName: s!("EngWindowClass"),
            ..Default::default()
        };

        let atom = unsafe { RegisterClassExA(&wc) };
        debug_assert_ne!(atom, 0);

        let hwnd: HWND = unsafe {
            CreateWindowExA(
                WINDOW_EX_STYLE::default(),
                s!("EngWindowClass"),
                s!("Engine"),
                WS_OVERLAPPEDWINDOW,
                0,
                0,
                size_x,
                size_y,
                None, // no parent window
                None, // no menus
                instance,
                Some(handler_ptr as *const c_void),
            )
        };

        Ok(WinAPIWindow { hwnd, handler_ptr })
    }

    pub fn show(&self) {
        unsafe { ShowWindow(self.hwnd, SW_SHOW) };
    }
}

extern "system" fn wndproc_bootstrap(
    window: HWND,
    message: u32,
    wparam: WPARAM,
    lparam: LPARAM,
) -> LRESULT {
    match message {
        WM_CREATE => {
            unsafe {
                let create_struct: &CREATESTRUCTA = transmute(lparam);
                SetWindowLongPtrA(window, GWLP_USERDATA, create_struct.lpCreateParams as isize);
                SetWindowLongPtrA(window, GWLP_WNDPROC, wndproc_main as usize as isize);
            }
            LRESULT::default()
        }
        _ => unsafe { DefWindowProcA(window, message, wparam, lparam) },
    }
}

extern "system" fn wndproc_main(
    window: HWND,
    message: u32,
    wparam: WPARAM,
    lparam: LPARAM,
) -> LRESULT {
    match message {
        WM_DESTROY => {
            unsafe { PostQuitMessage(0) };
            LRESULT::default()
        }
        _ => {
            let ptr: *mut MultiplexWinProc =
                unsafe { GetWindowLongPtrA(window, GWLP_USERDATA) as *mut MultiplexWinProc };
            let result = unsafe { ptr.as_mut() }
                .expect("missing windproc ptr")
                .wndproc(window, message, wparam, lparam);
            if !result {
                unsafe { DefWindowProcA(window, message, wparam, lparam) }
            } else {
                LRESULT::default()
            }
        }
    }
}
