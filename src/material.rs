pub enum ShaderInputDataType {
    Texture2d,
    Texture3d,
}

pub struct ShaderInput {
    data_type: ShaderInputDataType,
    slot: usize,
}

pub struct PixelShader {
    pub shader_path: String,
    pub alpha_blending: bool,
    pub inputs: Vec<ShaderInput>,
}

impl PixelShader {
    pub fn new<S: Into<String>>(shader_path: S) -> PixelShader {
        return PixelShader {
            shader_path: shader_path.into(),
            alpha_blending: false,
            inputs: Vec::new(),
        };
    }
}

pub struct Material<'a> {
    pub shader: &'a PixelShader,
}

impl<'a> Material<'a> {
    pub fn new(shader: &'a PixelShader) -> Material<'a> {
        return Material { shader };
    }
}
