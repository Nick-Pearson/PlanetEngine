mod graphics;
mod instance;
mod material;
mod mesh;

#[cfg(windows)]
use crate::graphics::d3dgraphics::D3DGraphics;
use crate::graphics::{CreateRenderer, RenderQueueItems, Renderer};
use crate::instance::MeshInstance;
use crate::material::{Material, PixelShader};
use glam::Quat;
use instance::Transform;
use mesh::Mesh;
use windows::{
    core::*, Win32::Foundation::*, Win32::System::LibraryLoader::*,
    Win32::UI::WindowsAndMessaging::*,
};

use std::f32::consts::FRAC_PI_2;
use std::ffi::c_void;
use std::mem::transmute;

trait WinProc {
    fn wndproc(&self, window: HWND, message: u32, wparam: WPARAM, lparam: LPARAM) -> bool;
}

struct InputWinProc {}

impl WinProc for InputWinProc {
    fn wndproc(&self, _window: HWND, _message: u32, _wparam: WPARAM, _lparam: LPARAM) -> bool {
        false
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
    fn wndproc(&self, window: HWND, message: u32, wparam: WPARAM, lparam: LPARAM) -> bool {
        for handler in self.handlers.iter() {
            let result = handler.as_ref().wndproc(window, message, wparam, lparam);
            if result {
                return true;
            }
        }
        false
    }
}

struct Window {
    hwnd: HWND,
}

impl Window {
    pub fn new(size_x: i32, size_y: i32) -> Result<Window> {
        let instance = unsafe { GetModuleHandleA(None)? };

        let mut handler = Box::new(MultiplexWinProc::new());
        handler.add_handler(Box::new(InputWinProc {}));
        let handler_ptr = Box::into_raw(handler) as *const c_void;

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
                Some(handler_ptr),
            )
        };

        Ok(Window { hwnd })
    }

    pub fn show(&self) {
        unsafe { ShowWindow(self.hwnd, SW_SHOW) };
    }
}

struct Engine<'a> {
    window: Window,
    renderer: &'a mut dyn Renderer,
    running: bool,
}

impl<'a> Engine<'a> {
    pub fn new(window: Window, renderer: &'a mut dyn Renderer) -> Engine {
        Engine {
            window,
            renderer,
            running: true,
        }
    }

    pub fn run(&mut self) {
        while self.running {
            self.pump_windows_messages();
            self.renderer.render_frame();
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
            let ptr: *const MultiplexWinProc =
                unsafe { GetWindowLongPtrA(window, GWLP_USERDATA) as *const MultiplexWinProc };
            let result = unsafe { ptr.as_ref() }
                .expect("missing windproc ptr")
                .wndproc(window, message, wparam, lparam);
            if !result {
                unsafe { DefWindowProcA(window, message, wparam, lparam) }
            } else {
                LRESULT(1)
            }
        }
    }
}

fn setup_scene(renderer: &mut dyn Renderer) {
    let pixel_shader = PixelShader::new("ps/FallbackShader.hlsl");
    let standard_material = Material::new(&pixel_shader);

    let floor_plane = Mesh::new_plane(100000.0);
    let mut floor = MeshInstance::new(&floor_plane, &standard_material);
    floor.rotate(Quat::from_euler(
        glam::EulerRot::XYZ,
        FRAC_PI_2,
        FRAC_PI_2,
        -FRAC_PI_2,
    ));

    // std::shared_ptr<Entity> planeEntity = scene_->SpawnEntity("wall");
    // planeEntity->Rotate(Vector{ 0.0f, 160.0f, 0.0f });
    // planeEntity->Translate(Vector{ -4.0f, -2.0f, -2.0f });
    // planeEntity->AddComponent<MeshComponent>(Primitives::Plane(2.0f), texturedMaterial);

    let mut queue = RenderQueueItems::empty();
    queue.new_meshes.push(&floor);
    renderer.apply(queue);
}

fn main() {
    let window = Window::new(1280, 720).unwrap();
    window.show();

    let graphics = D3DGraphics::new().unwrap();
    let mut renderer = graphics.create_renderer(&window.hwnd).unwrap();
    renderer.render_frame();

    setup_scene(&mut renderer);

    Engine::new(window, &mut renderer).run();
}
