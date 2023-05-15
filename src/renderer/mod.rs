
#[cfg(windows)]
mod d3drenderer;

pub trait Renderer
{    
    fn render_frame(&self);
}

pub fn default_renderer() -> impl Renderer {
    #[cfg(windows)]
    return d3drenderer::new_renderer();
}