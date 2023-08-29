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
use std::mem::transmute;

struct Window {
    hwnd: HWND,
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

struct Engine<'a> {
    window: Window,
    renderer: &'a dyn Renderer,
    running: bool,
}

impl<'a> Engine<'a> {
    pub fn new(window: Window, renderer: &'a dyn Renderer) -> Engine {
        return Engine {
            window: window,
            renderer: renderer,
            running: true,
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

extern "system" fn wndproc(window: HWND, message: u32, wparam: WPARAM, lparam: LPARAM) -> LRESULT {
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
        _ => unsafe { DefWindowProcA(window, message, wparam, lparam) },
    }
}

fn setup_scene<'a>(renderer: &mut dyn Renderer) {
    let pixel_shader = PixelShader::new("ps/PixelShader.hlsl");
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

    Engine::new(window, &renderer).run();
}
