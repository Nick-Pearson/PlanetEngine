use crate::material::PixelShader;
use glam::{Mat4, Vec3A};
use std::mem::size_of;

use windows::Win32::Graphics::{Direct3D::ID3DBlob, Direct3D12::*};

#[repr(C)]
pub struct D3DSlowVSConstants {
    world: Mat4,
    view: Mat4,
}

impl D3DSlowVSConstants {
    pub const SIZE_32_BIT: u32 = (size_of::<Self>() / size_of::<u32>()) as u32;
}

#[repr(C)]
pub struct D3DFastConstants {
    model: Mat4,
    sun_dir: Vec3A,
    sun_sky_strength: f32,
    sun_col: Vec3A,
}

impl D3DFastConstants {
    pub const SIZE_32_BIT: u32 = (size_of::<Self>() / size_of::<u32>()) as u32;
}

pub struct D3DRootSignature {
    root_signature: ID3D12RootSignature,
}

impl D3DRootSignature {
    fn check_root_signature_features(device: &ID3D12Device2) -> D3D12_FEATURE_DATA_ROOT_SIGNATURE {
        let mut feature_data = D3D12_FEATURE_DATA_ROOT_SIGNATURE {
            HighestVersion: D3D_ROOT_SIGNATURE_VERSION_1_1,
        };

        let result = unsafe {
            let ptr = &mut feature_data as *mut _ as *mut std::ffi::c_void;
            let size = size_of::<D3D12_FEATURE_DATA_ROOT_SIGNATURE>() as u32;
            device.CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, ptr, size)
        };
        if result.is_err() {
            feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }
        feature_data
    }

    fn init_as_constants(
        num_32bit_values: u32,
        shader_register: u32,
        register_space: u32,
        visibility: D3D12_SHADER_VISIBILITY,
    ) -> D3D12_ROOT_PARAMETER1 {
        let constants = D3D12_ROOT_CONSTANTS {
            ShaderRegister: shader_register,
            RegisterSpace: register_space,
            Num32BitValues: num_32bit_values,
        };
        D3D12_ROOT_PARAMETER1 {
            ParameterType: D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
            Anonymous: D3D12_ROOT_PARAMETER1_0 {
                Constants: constants,
            },
            ShaderVisibility: visibility,
        }
    }

    fn init_as_descriptor_table(
        range_type: D3D12_DESCRIPTOR_RANGE_TYPE,
        num_descriptors: usize,
        shader_visibility: D3D12_SHADER_VISIBILITY,
    ) -> D3D12_ROOT_PARAMETER1 {
        let ranges = vec![D3D12_DESCRIPTOR_RANGE1 {
            RangeType: range_type,
            NumDescriptors: num_descriptors as u32,
            BaseShaderRegister: 0,
            RegisterSpace: 0,
            Flags: D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
            OffsetInDescriptorsFromTableStart: D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
        }];
        let descriptor_table = D3D12_ROOT_DESCRIPTOR_TABLE1 {
            NumDescriptorRanges: 1,
            pDescriptorRanges: ranges.as_ptr(),
        };
        D3D12_ROOT_PARAMETER1 {
            ParameterType: D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
            Anonymous: D3D12_ROOT_PARAMETER1_0 {
                DescriptorTable: descriptor_table,
            },
            ShaderVisibility: shader_visibility,
        }
    }

    fn serialize_blob(
        params: &Vec<D3D12_ROOT_PARAMETER1>,
        samplers: &Vec<D3D12_STATIC_SAMPLER_DESC>,
        flags: D3D12_ROOT_SIGNATURE_FLAGS,
    ) -> ::windows::core::Result<ID3DBlob> {
        let desc = D3D12_ROOT_SIGNATURE_DESC1 {
            NumParameters: params.len() as u32,
            pParameters: params.as_ptr(),
            NumStaticSamplers: samplers.len() as u32,
            pStaticSamplers: samplers.as_ptr(),
            Flags: flags,
        };

        let versioned_desc = D3D12_VERSIONED_ROOT_SIGNATURE_DESC {
            Version: D3D_ROOT_SIGNATURE_VERSION_1_1,
            Anonymous: D3D12_VERSIONED_ROOT_SIGNATURE_DESC_0 { Desc_1_1: desc },
        };

        let mut blob_opt = None;
        let result =
            unsafe { D3D12SerializeVersionedRootSignature(&versioned_desc, &mut blob_opt, None) };
        result.map(|_| blob_opt.unwrap())
    }

    pub fn from_pixel_shader(
        pixel_shader: &PixelShader,
        device: &ID3D12Device2,
    ) -> D3DRootSignature {
        let _feature_data: D3D12_FEATURE_DATA_ROOT_SIGNATURE =
            Self::check_root_signature_features(device);

        let mut params: Vec<D3D12_ROOT_PARAMETER1> = Vec::new();
        params.push(Self::init_as_constants(
            D3DSlowVSConstants::SIZE_32_BIT,
            0,
            0,
            D3D12_SHADER_VISIBILITY_VERTEX,
        ));
        params.push(Self::init_as_constants(
            D3DFastConstants::SIZE_32_BIT,
            1,
            0,
            D3D12_SHADER_VISIBILITY_ALL,
        ));

        let num_inputs = pixel_shader.inputs.len();
        if num_inputs > 0 {
            params.push(Self::init_as_descriptor_table(
                D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                num_inputs,
                D3D12_SHADER_VISIBILITY_PIXEL,
            ));
        }

        // create a static sampler
        let sampler = D3D12_STATIC_SAMPLER_DESC {
            Filter: D3D12_FILTER_MIN_MAG_MIP_POINT,
            AddressU: D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            AddressV: D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            AddressW: D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            MipLODBias: 0.0,
            MaxAnisotropy: 0,
            ComparisonFunc: D3D12_COMPARISON_FUNC_NEVER,
            BorderColor: D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
            MinLOD: 0.0,
            MaxLOD: D3D12_FLOAT32_MAX,
            ShaderRegister: 0,
            RegisterSpace: 0,
            ShaderVisibility: D3D12_SHADER_VISIBILITY_PIXEL,
        };

        let flags: D3D12_ROOT_SIGNATURE_FLAGS =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
                | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
                | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
                | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        let samplers = vec![sampler];
        let blob = Self::serialize_blob(&params, &samplers, flags).unwrap();
        let signature = unsafe {
            device.CreateRootSignature(
                0,
                std::slice::from_raw_parts(blob.GetBufferPointer() as _, blob.GetBufferSize()),
            )
        }
        .unwrap();

        D3DRootSignature {
            root_signature: signature,
        }
    }

    pub fn get_signature(&self) -> ID3D12RootSignature {
        self.root_signature.clone()
    }
}
