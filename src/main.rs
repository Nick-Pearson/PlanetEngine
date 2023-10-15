mod graphics;
mod input;
mod instance;
mod material;
mod mesh;
mod windowing;

#[cfg(windows)]
use crate::graphics::d3dgraphics::D3DGraphics;
use crate::graphics::{CreateRenderer, RenderQueueItems, Renderer};
use crate::instance::MeshInstance;
use crate::material::{Material, PixelShader};
use glam::Quat;
use instance::Transform;
use mesh::Mesh;
use windowing::winapi::WinAPIWindow;

use windows::Win32::UI::WindowsAndMessaging::*;

use std::f32::consts::FRAC_PI_2;

struct Engine<'a> {
    window: WinAPIWindow,
    renderer: &'a mut dyn Renderer,
    running: bool,
}

impl<'a> Engine<'a> {
    pub fn new(window: WinAPIWindow, renderer: &'a mut dyn Renderer) -> Engine<'a> {
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
    let window = WinAPIWindow::new(1280, 720).unwrap();
    window.show();

    let graphics = D3DGraphics::new().unwrap();
    let mut renderer = graphics.create_renderer(&window.hwnd).unwrap();
    renderer.render_frame();

    setup_scene(&mut renderer);

    Engine::new(window, &mut renderer).run();
}
