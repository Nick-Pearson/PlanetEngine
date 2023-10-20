use crate::material::Material;
use crate::mesh::Mesh;
use glam::{Mat4, Quat, Vec3, Vec3A, Vec4};

pub trait Transform {
    fn translate<P: Into<Vec3>>(&mut self, delta: P);

    fn rotate<R: Into<Quat>>(&mut self, delta: R);

    fn rotate_around<P: Into<Vec3>, R: Into<Quat>>(&mut self, origin: P, delta: R);
}

#[derive(PartialEq, Debug, Clone, Copy)]
pub struct MatTransform {
    matrix: Mat4,
}

impl MatTransform {
    pub const IDENTITY: Self = Self {
        matrix: Mat4::IDENTITY,
    };

    pub fn from_position<P: Into<Vec3>>(position: P) -> MatTransform {
        Self {
            matrix: Mat4::from_translation(position.into()),
        }
    }

    pub fn position(&self) -> Vec3A {
        (self.matrix * Vec4::W).into()
    }

    pub fn rotation(&self) -> Quat {
        Quat::from_mat4(&self.matrix)
    }
}

impl From<MatTransform> for Mat4 {
    fn from(val: MatTransform) -> Self {
        val.matrix
    }
}

impl Transform for MatTransform {
    fn translate<P: Into<Vec3>>(&mut self, delta: P) {
        let translation_matrix = Mat4::from_translation(delta.into());
        self.matrix = translation_matrix * self.matrix;
    }

    fn rotate<R: Into<Quat>>(&mut self, delta: R) {
        self.rotate_around(self.position(), delta)
    }

    fn rotate_around<P: Into<Vec3>, R: Into<Quat>>(&mut self, origin: P, delta: R) {
        let point: Vec3 = origin.into();
        self.translate(-point);
        let rotation_matrix = Mat4::from_quat(delta.into());
        self.matrix = rotation_matrix * self.matrix;
        self.translate(point);
    }
}

pub struct MeshInstance<'a> {
    pub transform: MatTransform,
    pub mesh: &'a Mesh,
    pub material: &'a Material<'a>,
}

impl<'a> MeshInstance<'a> {
    pub fn new(mesh: &'a Mesh, material: &'a Material<'a>) -> MeshInstance<'a> {
        MeshInstance {
            transform: MatTransform::IDENTITY,
            mesh,
            material,
        }
    }
}

impl<'a> Transform for MeshInstance<'a> {
    fn translate<P: Into<Vec3>>(&mut self, delta: P) {
        self.transform.translate(delta);
    }

    fn rotate<R: Into<Quat>>(&mut self, delta: R) {
        self.transform.rotate(delta);
    }

    fn rotate_around<P: Into<Vec3>, R: Into<Quat>>(&mut self, origin: P, delta: R) {
        self.transform.rotate_around(origin, delta)
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
        assert_eq!(expected, a.position());
    }

    #[test]
    fn translate_moves_position() {
        let mut a = MatTransform::IDENTITY;

        a.translate([100.0, 200.0, 300.0]);

        let expected = Vec3A::new(100.0, 200.0, 300.0);
        assert_eq!(expected, a.position());
    }

    #[test]
    fn rotate_does_not_move_position() {
        let mut a = MatTransform::from_position([5.0, 1.0, 2.0]);

        a.rotate(Quat::from_rotation_x(FRAC_PI_2));

        let actual: Vec3 = a.position().into();
        assert_approx_eq!(5.0, actual.x);
        assert_approx_eq!(1.0, actual.y);
        assert_approx_eq!(2.0, actual.z);
    }

    #[test]
    fn rotate_around_origin_moves_position() {
        let mut a = MatTransform::from_position([5.0, 1.0, 2.0]);

        a.rotate_around(Vec3::new(0.0, 0.0, 0.0), Quat::from_rotation_x(FRAC_PI_2));

        let actual: Vec3 = a.position().into();
        assert_approx_eq!(5.0, actual.x);
        assert_approx_eq!(-2.0, actual.y);
        assert_approx_eq!(1.0, actual.z);
    }
}
