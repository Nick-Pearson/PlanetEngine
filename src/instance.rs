use crate::material::Material;
use crate::mesh::Mesh;
use glam::Mat3A;

pub struct MeshInstance<'a> {
    transform: Mat3A,
    mesh: &'a Mesh,
    material: &'a Material<'a>,
}
