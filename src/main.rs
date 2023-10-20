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
use glam::{Mat4, Quat, Vec3};
use input::{InputReader, KeyCode};
use instance::{MatTransform, Transform};

use mesh::Mesh;
use windowing::winapi::WinAPIWindow;
use windowing::Window;

use windows::Win32::UI::WindowsAndMessaging::*;

use std::f32::consts::FRAC_PI_2;
use std::time::Instant;

struct Camera {
    transform: MatTransform,
}

impl Camera {
    pub fn new() -> Camera {
        let mut transform = MatTransform::IDENTITY;
        transform.translate([0.0, 4.0, 10.0]);
        // transform.rotate(Quat::from_euler(glam::EulerRot::YXZ, PI, 0.0, 0.0));

        Camera { transform }
    }

    pub fn update(&mut self, delta_time: f32, input: &dyn InputReader) {
        let mut movement = Vec3::ZERO;
        let mut rotation_x = 0.0_f32;
        let mut rotation_y = 0.0_f32;

        if input.is_key_down(KeyCode::Character('W')) {
            movement += Vec3::new(0.0, 0.0, 1.0);
        } else if input.is_key_down(KeyCode::Character('S')) {
            movement += Vec3::new(0.0, 0.0, -1.0);
        }
        if input.is_key_down(KeyCode::Character('D')) {
            movement += Vec3::new(1.0, 0.0, 0.0);
        } else if input.is_key_down(KeyCode::Character('A')) {
            movement += Vec3::new(-1.0, 0.0, 0.0);
        }

        if input.is_key_down(KeyCode::UpArrow) {
            rotation_x += 1.0;
        } else if input.is_key_down(KeyCode::DownArrow) {
            rotation_x -= 1.0;
        }

        if input.is_key_down(KeyCode::LeftArrow) {
            rotation_y -= 1.0;
        } else if input.is_key_down(KeyCode::RightArrow) {
            rotation_y += 1.0;
        }

        if input.is_key_down(KeyCode::LeftShift) {
            movement *= 10.0;
            rotation_x *= 10.0;
            rotation_y *= 10.0;
        }

        const ROTATION_SPEED: f32 = 0.5;
        const MOVE_SPEED: f32 = 10.0;

        self.transform.rotate(Quat::from_euler(
            glam::EulerRot::YXZ,
            rotation_y * delta_time * ROTATION_SPEED,
            rotation_x * delta_time * ROTATION_SPEED,
            0.0,
        ));
        self.transform.translate(movement * delta_time * MOVE_SPEED);
    }
}

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
        let mut camera = Camera::new();
        let mut delta_time = 0.01_f32;
        let mut begin = Instant::now();
        while self.running {
            self.pump_windows_messages();
            camera.update(delta_time, self.window.input());
            self.renderer.render_frame(&camera.transform.into());

            let end = Instant::now();
            delta_time = (end - begin).as_secs_f32();
            begin = end;
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
    let pixel_shader = PixelShader::new("ps/PixelShader.hlsl");
    let standard_material = Material::new(&pixel_shader);

    let floor_plane = Mesh::new_plane(100000.0);
    let mut floor = MeshInstance::new(&floor_plane, &standard_material);
    floor.rotate(Quat::from_euler(
        glam::EulerRot::YXZ,
        FRAC_PI_2,
        FRAC_PI_2,
        -FRAC_PI_2,
    ));

    let wall_plane = Mesh::new_plane(2.0);
    let mut wall = MeshInstance::new(&wall_plane, &standard_material);
    wall.rotate(Quat::from_rotation_y(f32::to_radians(160.0)));
    wall.translate(Vec3::new(-4.0, -2.0, -2.0));

    let mut queue = RenderQueueItems::empty();
    queue.new_meshes.push(&floor);
    queue.new_meshes.push(&wall);
    renderer.apply(queue);
}

fn main() {
    simple_logger::init_with_level(log::Level::Info).unwrap();

    let window = WinAPIWindow::new(1280, 720).unwrap();
    window.show();

    let graphics = D3DGraphics::new().unwrap();
    let mut renderer = graphics.create_renderer(&window.hwnd).unwrap();
    renderer.render_frame(&Mat4::IDENTITY);

    setup_scene(&mut renderer);

    Engine::new(window, &mut renderer).run();
}
