use crate::mesh::MeshComponent;

#[cfg(windows)]
pub mod d3dgraphics;

pub struct RenderQueueItems<'a> {
    new_meshes: Vec<&'a MeshComponent<'a>>,
    updated_meshes: Vec<&'a MeshComponent<'a>>,
    removed_meshes: Vec<&'a MeshComponent<'a>>
}

pub trait Renderer
{
    fn apply(&self, items: RenderQueueItems);

    fn render_frame(&self);
}

pub trait CreateRenderer<'a>
{
    type Canvas;
    type Output;
    type Err;

    fn create_renderer(&'a self, canvas: &Self::Canvas) -> Result<Self::Output, Self::Err>;
}