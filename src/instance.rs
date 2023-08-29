use crate::material::Material;
use crate::mesh::Mesh;
use glam::{Mat4, Quat, Vec3, Vec3A, Vec4};

pub trait Transform {
    fn translate<P: Into<Vec3>>(&mut self, delta: P);

    fn rotate<R: Into<Quat>>(&mut self, delta: R);
}

#[derive(PartialEq, Debug)]
pub struct MatTransform {
    matrix: Mat4,
}

impl MatTransform {
    pub const IDENTITY: Self = Self {
        matrix: Mat4::IDENTITY,
    };

    pub fn from_position<P: Into<Vec3>>(position: P) -> MatTransform {
        return Self {
            matrix: Mat4::from_translation(position.into()),
        };
    }

    pub fn get_position(&self) -> Vec3A {
        return (self.matrix * Vec4::W).into();
    }
}

impl Transform for MatTransform {
    fn translate<P: Into<Vec3>>(&mut self, delta: P) {
        let translation_matrix = Mat4::from_translation(delta.into());
        self.matrix = self.matrix * translation_matrix;
    }

    fn rotate<R: Into<Quat>>(&mut self, delta: R) {
        let rotation_matrix = Mat4::from_quat(delta.into());
        self.matrix = rotation_matrix * self.matrix;
    }
}

pub struct MeshInstance<'a> {
    transform: MatTransform,
    mesh: &'a Mesh,
    material: &'a Material<'a>,
}

impl<'a> MeshInstance<'a> {
    pub fn new(mesh: &'a Mesh, material: &'a Material<'a>) -> MeshInstance<'a> {
        return MeshInstance {
            transform: MatTransform::IDENTITY,
            mesh: mesh,
            material: material,
        };
    }
}

impl<'a> Transform for MeshInstance<'a> {
    fn translate<P: Into<Vec3>>(&mut self, delta: P) {
        self.transform.translate(delta);
    }

    fn rotate<R: Into<Quat>>(&mut self, delta: R) {
        self.transform.rotate(delta);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use assert_approx_eq::assert_approx_eq;
    use std::f32::consts::FRAC_PI_2;

    #[test]
    fn construct_from_position() {
        let a = MatTransform::from_position([50.0, 100.0, 200.0]);

        let expected = Vec3A::new(50.0, 100.0, 200.0);
        assert_eq!(expected, a.get_position());
    }

    #[test]
    fn translate_moves_position() {
        let mut a = MatTransform::IDENTITY;

        a.translate([100.0, 200.0, 300.0]);

        let expected = Vec3A::new(100.0, 200.0, 300.0);
        assert_eq!(expected, a.get_position());
    }

    #[test]
    fn rotate_moves_position() {
        let mut a = MatTransform::from_position([5.0, 1.0, 2.0]);

        a.rotate(Quat::from_rotation_x(FRAC_PI_2));

        let actual: Vec3 = a.get_position().into();
        assert_approx_eq!(5.0, actual.x);
        assert_approx_eq!(-2.0, actual.y);
        assert_approx_eq!(1.0, actual.z);
    }
}
