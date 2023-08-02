mod graphics;
mod mesh;

use mesh::Mesh;
use crate::graphics::CreateRenderer;
#[cfg(windows)]
use crate::graphics::d3dgraphics::{
    D3DGraphics,
    D3DRenderer
};
use graphics::Renderer;
use windows::{
    core::*,
    Win32::Foundation::*,
    Win32::System::LibraryLoader::*,
    Win32::UI::WindowsAndMessaging::*,
};

use std::{mem::transmute};

struct Window
{
    hwnd: HWND
}

impl Window {
    pub fn new(size_x: i32, size_y: i32) -> Result<Window> {
        let instance = unsafe { GetModuleHandleA(None)? };
    
        let wc: WNDCLASSEXA = WNDCLASSEXA {
            cbSize: std::mem::size_of::<WNDCLASSEXA>() as u32,
            style: CS_OWNDC,
            lpfnWndProc: Some(wndproc),
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
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                size_x,
                size_y,
                None, // no parent window
                None, // no menus
                instance,
                None,
            )
        };

        return Ok(Window { hwnd });
    }

    pub fn show(&self) -> () {
        unsafe { ShowWindow(self.hwnd, SW_SHOW) };
    }
}

struct Engine<'a>
{
    window: Window,
    renderer: &'a dyn Renderer,
    running: bool,
}

impl<'a> Engine<'a> {
    pub fn new(window: Window, renderer: &'a dyn Renderer) -> Engine {
        return Engine {
            window: window,
            renderer: renderer,
            running: true
        };
    }

    pub fn run(&mut self) -> () {
        while self.running {
            self.pump_windows_messages();
        }
    }

    fn pump_windows_messages(&mut self) {
        let mut message = MSG::default();

        while unsafe { PeekMessageA(&mut message, None, 0, 0, PM_REMOVE) }.into() {
            unsafe {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            }

            if message.message == WM_QUIT {
                self.running = false;
            }
        }
    }
}

extern "system" fn wndproc(
    window: HWND,
    message: u32,
    wparam: WPARAM,
    lparam: LPARAM,
) -> LRESULT {
    match message {
        WM_CREATE => {
            unsafe {
                let create_struct: &CREATESTRUCTA = transmute(lparam);
                SetWindowLongPtrA(window, GWLP_USERDATA, create_struct.lpCreateParams as _);
            }
            LRESULT::default()
        }
        WM_DESTROY => {
            unsafe { PostQuitMessage(0) };
            LRESULT::default()
        }
        _ => {
            unsafe { DefWindowProcA(window, message, wparam, lparam) }
        }
    }
}

fn main() {
    let window = Window::new(1280, 720)
        .unwrap();
    window.show();

    let a = Mesh{};

    let graphics = D3DGraphics::new()
        .unwrap();
    let renderer = graphics.create_renderer(&window.hwnd)
        .unwrap();
    renderer.render_frame();
    
    Engine::new(window, &renderer).run();
}