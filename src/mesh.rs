use glam::{Vec2, Vec3A};

pub struct Vertex {
    position: Vec3A,
    normal: Vec3A,
    tex_coords: Vec2,
}

impl Vertex {
    pub fn new(position: [f32; 3], normal: [f32; 3], tex_coords: [f32; 2]) -> Vertex {
        return Vertex {
            position: Vec3A::from_array(position),
            normal: Vec3A::from_array(normal),
            tex_coords: Vec2::from_array(tex_coords),
        };
    }
}

pub struct Mesh {
    verticies: Vec<Vertex>,
    triangles: Vec<u16>,
}

impl Mesh {
    pub fn new_plane(scale: f32) -> Mesh {
        let norm = [0.0, 0.0, 1.0];
        let v = vec![
            Vertex::new([scale * 1.0, scale * -1.0, 0.0], norm, [1.0, 0.0]),
            Vertex::new([scale * -1.0, scale * 1.0, 0.0], norm, [0.0, 1.0]),
            Vertex::new([scale * 1.0, scale * 1.0, 0.0], norm, [1.0, 1.0]),
            Vertex::new([scale * -1.0, scale * -1.0, 0.0], norm, [0.0, 0.0]),
        ];
        let t = vec![0, 1, 2, 0, 3, 1];
        return Mesh {
            verticies: v,
            triangles: t,
        };
    }
}
