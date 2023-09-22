use std::mem::size_of;

use windows::{
    core::Result,
    Win32::Graphics::{Direct3D12::*, Dxgi::Common::*},
};

use super::d3dresources::{BufferDesc, D3DBuffer, D3DResources};

trait VertexLayout {
    fn len(&self) -> usize;

    fn encode_vert<F>(&self, vert: &crate::mesh::Vertex, consumer: F)
    where
        F: FnMut(&[u8]);
}

struct PosNormUVVertexLayout;
impl PosNormUVVertexLayout {
    fn encode_vec<F>(vec: &glam::Vec3, mut func: F)
    where
        F: FnMut(&[u8]),
    {
        func(&vec.x.to_ne_bytes());
        func(&vec.y.to_ne_bytes());
        func(&vec.z.to_ne_bytes());
    }

    fn encode_vec2<F>(vec: &glam::Vec2, mut func: F)
    where
        F: FnMut(&[u8]),
    {
        func(&vec.x.to_ne_bytes());
        func(&vec.y.to_ne_bytes());
    }
}

impl VertexLayout for PosNormUVVertexLayout {
    fn encode_vert<F>(&self, vert: &crate::mesh::Vertex, mut consumer: F)
    where
        F: FnMut(&[u8]),
    {
        Self::encode_vec(&vert.position, &mut consumer);
        Self::encode_vec(&vert.normal, &mut consumer);
        Self::encode_vec2(&vert.tex_coords, &mut consumer);
    }

    fn len(&self) -> usize {
        std::mem::size_of::<f32>() * 8
    }
}

pub struct D3DMesh {
    pub loaded: bool,
    first_bind: bool,

    vertex_buffer: D3DBuffer,
    vertex_buffer_view: D3D12_VERTEX_BUFFER_VIEW,

    num_indicies: usize,
    triangle_buffer: D3DBuffer,
    triangle_buffer_view: D3D12_INDEX_BUFFER_VIEW,
}

impl D3DMesh {
    pub fn load(mesh: &crate::mesh::Mesh, resources: &D3DResources) -> Result<D3DMesh> {
        let vertex_format = PosNormUVVertexLayout;

        let mut vert_data = Vec::with_capacity(vertex_format.len() * mesh.verticies.len());
        for vert in &mesh.verticies {
            vertex_format.encode_vert(vert, |b| vert_data.extend_from_slice(b))
        }
        let vert_desc = BufferDesc {
            data: vert_data,
            element_size: vertex_format.len(),
            ..Default::default()
        };
        let vertex_buffer = resources.initialise_buffer(&vert_desc)?;
        let vertex_buffer_view = D3D12_VERTEX_BUFFER_VIEW {
            BufferLocation: vertex_buffer.get_gpu_virtual_address(),
            SizeInBytes: (vert_desc.element_size * vert_desc.data.len()) as u32,
            StrideInBytes: vert_desc.element_size as u32,
        };

        let tri_data: Vec<u8> = mesh.indicies.iter().flat_map(|i| i.to_ne_bytes()).collect();
        let tri_desc = BufferDesc {
            data: tri_data,
            element_size: size_of::<u16>(),
            ..Default::default()
        };
        let triangle_buffer = resources.initialise_buffer(&tri_desc)?;
        let triangle_buffer_view = D3D12_INDEX_BUFFER_VIEW {
            BufferLocation: triangle_buffer.get_gpu_virtual_address(),
            SizeInBytes: (tri_desc.element_size * tri_desc.data.len()) as u32,
            Format: DXGI_FORMAT_R16_UINT,
        };

        Ok(D3DMesh {
            loaded: false,
            first_bind: false,

            vertex_buffer,
            vertex_buffer_view,

            num_indicies: mesh.indicies.len(),
            triangle_buffer,
            triangle_buffer_view,
        })
    }

    pub fn draw(&mut self, command_list: ID3D12GraphicsCommandList) {
        if self.first_bind {
            self.first_bind = false;

            self.vertex_buffer.transition_to(
                &command_list,
                D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            );
            self.triangle_buffer
                .transition_to(&command_list, D3D12_RESOURCE_STATE_INDEX_BUFFER);
        }

        unsafe {
            command_list.IASetVertexBuffers(0, Some(&[self.vertex_buffer_view]));
            command_list.IASetIndexBuffer(Some(&self.triangle_buffer_view));
        };
        let instance_count = 1;
        let start_index = 0;
        let start_vertex = 0;
        let start_instance = 0;
        unsafe {
            command_list.DrawIndexedInstanced(
                self.num_indicies as u32,
                instance_count,
                start_index,
                start_vertex,
                start_instance,
            )
        };
    }
}
