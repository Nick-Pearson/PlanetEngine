
use crate::renderer::Renderer;

pub struct D3DRenderer
{

}

impl Renderer for D3DRenderer
{
    fn render_frame(&self) {
    }
}

pub fn new_renderer() -> D3DRenderer
{
    return D3DRenderer {};
}