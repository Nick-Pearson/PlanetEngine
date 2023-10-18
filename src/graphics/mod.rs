use glam::Mat4;

use crate::instance::MeshInstance;

#[cfg(windows)]
pub mod d3dgraphics;

pub struct RenderQueueItems<'a> {
    pub new_meshes: Vec<&'a MeshInstance<'a>>,
    pub updated_meshes: Vec<&'a MeshInstance<'a>>,
    pub removed_meshes: Vec<&'a MeshInstance<'a>>,
}

impl<'a> RenderQueueItems<'a> {
    pub fn empty() -> RenderQueueItems<'a> {
        RenderQueueItems {
            new_meshes: Vec::new(),
            updated_meshes: Vec::new(),
            removed_meshes: Vec::new(),
        }
    }
}

pub trait Renderer {
    fn apply(&mut self, items: RenderQueueItems);

    fn render_frame(&mut self, camera_transform: &Mat4);
}

pub trait CreateRenderer<'a> {
    type Canvas;
    type Output;
    type Err;

    fn create_renderer(&'a self, canvas: &Self::Canvas) -> Result<Self::Output, Self::Err>;
}
