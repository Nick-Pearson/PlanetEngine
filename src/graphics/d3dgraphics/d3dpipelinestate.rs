use std::{ffi::c_void, mem::ManuallyDrop};

use windows::{Win32::Graphics::Direct3D12::*, Win32::Graphics::Dxgi::Common::*};

use super::{
    d3drootsignature::D3DRootSignature,
    d3dshader::{D3DPixelShader, D3DVertexShader},
};

pub struct D3DPipelineState {
    pipeline_state: ID3D12PipelineState,
}

macro_rules! subobj {
    ($name:ident, $kind: expr, $typ: ty) => {
        #[allow(dead_code)]
        #[repr(align(64))]
        struct $name {
            kind: D3D12_PIPELINE_STATE_SUBOBJECT_TYPE,
            payload: $typ,
        }

        impl From<$typ> for $name {
            fn from(value: $typ) -> Self {
                return Self {
                    kind: $kind,
                    payload: value,
                };
            }
        }
    };
}
subobj!(
    PipelineStateRootSignature,
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE,
    ID3D12RootSignature
);
subobj!(
    PipelineStateInputLayout,
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT,
    D3D12_INPUT_LAYOUT_DESC
);
subobj!(
    PipelineStatePrimitiveTopology,
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE
);
subobj!(
    PipelineStateRasterizer,
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER,
    D3D12_RASTERIZER_DESC
);
subobj!(
    PipelineStateVS,
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS,
    D3D12_SHADER_BYTECODE
);
subobj!(
    PipelineStatePS,
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS,
    D3D12_SHADER_BYTECODE
);
subobj!(
    PipelineStateDepthStencil,
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL,
    D3D12_DEPTH_STENCIL_DESC
);
subobj!(
    PipelineStateDepthStencilFormat,
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT,
    DXGI_FORMAT
);
subobj!(
    PipelineStateRenderTargetFormats,
    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS,
    D3D12_RT_FORMAT_ARRAY
);

#[repr(C)]
struct PipelineStateStream {
    root_signature: PipelineStateRootSignature,
    input_layout: PipelineStateInputLayout,
    primitive_topology_type: PipelineStatePrimitiveTopology,
    rasertizer: PipelineStateRasterizer,
    vs: PipelineStateVS,
    ps: PipelineStatePS,
    depth_stencil: PipelineStateDepthStencil,
    dsv_format: PipelineStateDepthStencilFormat,
    rtv_formats: PipelineStateRenderTargetFormats,
}

impl D3DPipelineState {
    fn create_pipeline_state(
        device: &ID3D12Device2,
        pixel_shader: &D3DPixelShader,
        vertex_shader: &D3DVertexShader,
        root_signature: &D3DRootSignature,
    ) -> windows::core::Result<ID3D12PipelineState> {
        let mut rtv_formats = D3D12_RT_FORMAT_ARRAY {
            NumRenderTargets: 3,
            RTFormats: [DXGI_FORMAT_UNKNOWN; 8],
        };
        rtv_formats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

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

        let mut state = PipelineStateStream {
            root_signature: signature.into(),
            input_layout: (*vertex_shader.get_input_layout()).into(),
            primitive_topology_type: D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE.into(),
            rasertizer: rasterizer.into(),
            vs: (*vertex_shader.get_bytecode()).into(),
            ps: (*pixel_shader.get_bytecode()).into(),
            depth_stencil: depth_stencil.into(),
            dsv_format: DXGI_FORMAT_D32_FLOAT.into(),
            rtv_formats: rtv_formats.into(),
        };
        let desc = D3D12_PIPELINE_STATE_STREAM_DESC {
            SizeInBytes: std::mem::size_of::<PipelineStateStream>(),
            pPipelineStateSubobjectStream: &mut state as *mut _ as *mut c_void,
        };
        return unsafe { device.CreatePipelineState(&desc) };
    }

    pub fn compile_for_mesh(
        device: &ID3D12Device2,
        ps: &crate::material::PixelShader,
        root_signature: &D3DRootSignature,
    ) -> std::result::Result<D3DPipelineState, String> {
        let pixel_shader = D3DPixelShader::compile(&ps.shader_path)?;
        let vertex_shader = D3DVertexShader::compile("vs/VertexShader.hlsl")?;

        let pipeline_state =
            Self::create_pipeline_state(device, &pixel_shader, &vertex_shader, root_signature)
                .unwrap();
        return Ok(D3DPipelineState { pipeline_state });
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn subobjects_have_correct_alignment() {
    }
}