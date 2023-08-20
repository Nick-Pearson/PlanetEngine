use std::{
    collections::HashMap,
    ops::{Mul, Sub},
};

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
    indicies: Vec<u16>,
}

impl Mesh {
    pub fn scale(&mut self, scale: [f32; 3]) {
        for vert in self.verticies.iter_mut() {
            vert.position = vert.position.mul(Vec3A::from_array(scale));
        }
    }

    pub fn num_triangles(&self) -> usize {
        return self.indicies.len() / 3;
    }

    pub fn recalculate_normals(&mut self) {
        // reset all normals
        for vert in self.verticies.iter_mut() {
            vert.normal = Vec3A::ZERO;
        }

        for i in (0..self.indicies.len()).step_by(3)
        // for (int i = 0; i < numTris; ++i)
        {
            let v1 = self.indicies[i] as usize;
            let v2 = self.indicies[i + 1] as usize;
            let v3 = self.indicies[i + 2] as usize;

            let side1 = self.verticies[v2].position.sub(self.verticies[v1].position);
            let side2 = self.verticies[v3].position.sub(self.verticies[v1].position);

            let cross = side2.cross(side1).normalize();

            self.verticies[v1].normal += cross;
            self.verticies[v2].normal += cross;
            self.verticies[v3].normal += cross;
        }

        // average all normals
        for vert in self.verticies.iter_mut() {
            vert.normal = vert.normal.normalize();
        }
    }
}

// primitives
impl Mesh {
    pub fn new_cube(scale: f32) -> Mesh {
        let zero = [0.0, 0.0, 0.0];
        let v = vec![
            Vertex::new([-1.0, -1.0, 1.0], zero, [0.0, 1.0]),
            Vertex::new([1.0, -1.0, 1.0], zero, [1.0, 1.0]),
            Vertex::new([-1.0, -1.0, -1.0], zero, [0.0, 0.0]),
            Vertex::new([1.0, -1.0, -1.0], zero, [1.0, 0.0]),
            Vertex::new([-1.0, 1.0, 1.0], zero, [0.0, 1.0]),
            Vertex::new([1.0, 1.0, 1.0], zero, [1.0, 1.0]),
            Vertex::new([-1.0, 1.0, -1.0], zero, [0.0, 0.0]),
            Vertex::new([1.0, 1.0, -1.0], zero, [1.0, 0.0]),
        ];
        let t = vec![
            // front
            0, 1, 5, 0, 5, 4, // back 2,3,6,7
            3, 2, 6, 3, 6, 7, // top
            6, 4, 5, 6, 5, 7, // bottom 0,1,2,3
            0, 3, 1, 0, 2, 3, // right
            5, 1, 3, 5, 3, 7, // left 6,4,2,0
            0, 6, 2, 0, 4, 6,
        ];
        let mut cube = Mesh {
            verticies: v,
            indicies: t,
        };
        cube.recalculate_normals();
        cube.scale([scale, scale, scale]);
        return cube;
    }

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
            indicies: t,
        };
    }

    pub fn new_ico_hemisphere(scale: f32, order: usize) -> Mesh {
        let rootFive: f32 = 5.0_f32.sqrt();
        let f: f32 = (1.0 + rootFive) / 2.0;

        let mut verts = vec![
            Vertex::new([-1.0, f, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0]),
            Vertex::new([1.0, f, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0]),
            Vertex::new([0.0, -1.0, f], [0.0, 0.0, 0.0], [0.0, 0.0]),
            Vertex::new([0.0, 1.0, f], [0.0, 0.0, 0.0], [0.0, 0.0]),
            Vertex::new([0.0, -1.0, -f], [0.0, 0.0, 0.0], [0.0, 0.0]),
            Vertex::new([0.0, 1.0, -f], [0.0, 0.0, 0.0], [0.0, 0.0]),
            Vertex::new([f, 0.0, -1.0], [0.0, 0.0, 0.0], [0.0, 0.0]),
            Vertex::new([f, 0.0, 1.0], [0.0, 0.0, 0.0], [0.0, 0.0]),
            Vertex::new([-f, 0.0, -1.0], [0.0, 0.0, 0.0], [0.0, 0.0]),
            Vertex::new([-f, 0.0, 1.0], [0.0, 0.0, 0.0], [0.0, 0.0]),
        ];

        let mut indicies = vec![
            0, 9, 3, 0, 3, 1, 0, 1, 5, 0, 5, 8, 0, 8, 9, 3, 9, 2, 1, 3, 7, 5, 1, 6, 8, 5, 4, 7, 6,
            1, 2, 7, 3, 6, 4, 5,
        ];

        let mut mid_cache = HashMap::new();

        let mut add_mid_point = |a: u16, b: u16| -> u16 {
            let key = (a as u32) | ((b as u32) << 16);
            let inverse_key = (b as u32) | ((a as u32) << 16);

            let result = *mid_cache.entry(key).or_insert_with(|| {
                let v = verts.len() as u16;
                verts.push(Vertex {
                    position: (verts[a as usize].position + verts[b as usize].position) / 2.0,
                    normal: Vec3A::ZERO,
                    tex_coords: Vec2::ZERO,
                });
                return v;
            });
            mid_cache.entry(inverse_key).or_insert(result);

            return result;
        };

        for _i in 0..order {
            // subdivide each triangle into 4 triangles
            let indicies_prev = indicies.clone();
            indicies = Vec::new();

            for k in (0..indicies_prev.len()).step_by(3) {
                let v1 = indicies_prev[k + 0];
                let v2 = indicies_prev[k + 1];
                let v3 = indicies_prev[k + 2];
                let a = add_mid_point(v1, v2);
                let b = add_mid_point(v2, v3);
                let c = add_mid_point(v3, v1);

                indicies.push(v1);
                indicies.push(a);
                indicies.push(c);

                indicies.push(v2);
                indicies.push(b);
                indicies.push(a);

                indicies.push(v3);
                indicies.push(c);
                indicies.push(b);

                indicies.push(a);
                indicies.push(b);
                indicies.push(c);
            }
        }

        for vert in verts.iter_mut() {
            vert.position = vert.position.normalize();
            vert.normal = vert.position;
            vert.position = vert.position.mul(scale);
        }
        return Mesh {
            verticies: verts,
            indicies: indicies,
        };
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use assert_approx_eq::assert_approx_eq;

    #[test]
    fn plane_has_correct_verts_and_tris() {
        let plane = Mesh::new_plane(200.0);

        assert_eq!(4, plane.verticies.len());
        assert_eq!(2, plane.num_triangles());
        for i in 0..4 {
            assert_eq!(80000.0, plane.verticies[i].position.length_squared());
        }
    }

    #[test]
    fn cube_has_correct_verts_and_tris() {
        let cube = Mesh::new_cube(200.0);

        assert_eq!(8, cube.verticies.len());
        assert_eq!(12, cube.num_triangles());
        for i in 0..8 {
            assert_eq!(120000.0, cube.verticies[i].position.length_squared());
        }
    }

    #[test]
    fn ico_hemisphere_level_one_has_correct_verts_and_tris() {
        let ico = Mesh::new_ico_hemisphere(100.0, 1);

        assert_eq!(31, ico.verticies.len());
        assert_eq!(48, ico.num_triangles());
        for i in 0..8 {
            assert_approx_eq!(100.0, ico.verticies[i].position.length(), 1e-5);
        }
    }
}
