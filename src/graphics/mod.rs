
#[cfg(windows)]
pub mod d3dgraphics;


pub trait Renderer
{
    fn render_frame(&self);
}

pub trait CreateRenderer<'a>
{
    type Canvas;
    type Output;
    type Err;

    fn create_renderer(&'a self, canvas: &Self::Canvas) -> Result<Self::Output, Self::Err>;
}