use std::mem::ManuallyDrop;

use windows::{Win32::Graphics::Direct3D12::*, Win32::Graphics::Dxgi::Common::*};

use super::{
    d3drootsignature::D3DRootSignature,
    d3dshader::{D3DPixelShader, D3DVertexShader},
};

pub struct D3DPipelineState {
    pipeline_state: ID3D12PipelineState,
}

impl D3DPipelineState {
    pub fn compile_for_mesh(
        device: &ID3D12Device2,
        ps: &crate::material::PixelShader,
        root_signature: &D3DRootSignature,
    ) -> std::result::Result<D3DPipelineState, String> {
        let pixel_shader = D3DPixelShader::compile(&ps.shader_path)?;
        let vertex_shader = D3DVertexShader::compile("vs/VertexShader.hlsl")?;

        let mut rtv_formats = [DXGI_FORMAT_UNKNOWN; 8];
        rtv_formats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        let depth_stencil = D3D12_DEPTH_STENCIL_DESC {
            DepthEnable: true.into(),
            DepthWriteMask: D3D12_DEPTH_WRITE_MASK_ALL,
            DepthFunc: D3D12_COMPARISON_FUNC_LESS,
            ..Default::default()
        };

        let rasterizer = D3D12_RASTERIZER_DESC {
            FillMode: D3D12_FILL_MODE_SOLID,
            CullMode: D3D12_CULL_MODE_BACK,
            ..Default::default()
        };

        let signature = root_signature.get_signature();
        let desc = D3D12_GRAPHICS_PIPELINE_STATE_DESC {
            pRootSignature: ManuallyDrop::new(Some(signature)),
            VS: pixel_shader.get_bytecode(),
            PS: vertex_shader.get_bytecode(),
            RasterizerState: rasterizer,
            DepthStencilState: depth_stencil,
            InputLayout: vertex_shader.get_input_layout(),
            PrimitiveTopologyType: D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            NumRenderTargets: 3,
            RTVFormats: rtv_formats,
            DSVFormat: DXGI_FORMAT_D32_FLOAT,
            ..Default::default()
        };
        let pipeline_state = unsafe { device.CreateGraphicsPipelineState(&desc) }.unwrap();
        return Ok(D3DPipelineState { pipeline_state });
    }
}
