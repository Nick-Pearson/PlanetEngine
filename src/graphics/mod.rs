use crate::instance::MeshInstance;

#[cfg(windows)]
pub mod d3dgraphics;

pub struct RenderQueueItems<'a> {
    new_meshes: Vec<&'a MeshInstance<'a>>,
    updated_meshes: Vec<&'a MeshInstance<'a>>,
    removed_meshes: Vec<&'a MeshInstance<'a>>,
}

pub trait Renderer {
    fn apply(&mut self, items: RenderQueueItems);

    fn render_frame(&mut self);
}

pub trait CreateRenderer<'a> {
    type Canvas;
    type Output;
    type Err;

    fn create_renderer(&'a self, canvas: &Self::Canvas) -> Result<Self::Output, Self::Err>;
}
