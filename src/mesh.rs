use glam::{
    Vec2, Vec3A, Mat3A
};

pub struct Vertex {
    position: Vec3A,
    normal: Vec3A,
    tex_coords: Vec2
}

pub struct Mesh {
  verticies: Vec<Vertex>,
  triangles: Vec<u16>  
}

pub struct MeshComponent<'a> {
    transform: Mat3A,
    mesh: &'a Mesh,
    // material
    // vertex shader
    // visible
}