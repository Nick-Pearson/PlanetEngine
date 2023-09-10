use std::{collections::HashMap, ffi::CStr};

use windows::{
    core::*,
    Win32::Graphics::Direct3D::*,
    Win32::Graphics::Direct3D12::*,
    Win32::Graphics::{Direct3D::Fxc::*, Dxgi::Common::*},
};

fn extract_compile_error(blob: Option<ID3DBlob>) -> String {
    match blob {
        None => "Unkown error".into(),
        Some(a) => unsafe { CStr::from_ptr(a.GetBufferPointer() as *const i8) }
            .to_str()
            .unwrap_or("Error message corrupted")
            .into(),
    }
}

// #[implement(ID3DInclude)]
// struct D3DIncludeHandler {}

// impl ID3DInclude_Impl for D3DIncludeHandler {
//     fn Open(
//         &self,
//         includetype: D3D_INCLUDE_TYPE,
//         pfilename: &::windows::core::PCSTR,
//         pparentdata: *const ::core::ffi::c_void,
//         ppdata: *mut *mut ::core::ffi::c_void,
//         pbytes: *mut u32,
//     ) -> ::windows::core::Result<()> {
//         todo!()
//     }

//     fn Close(&self, pdata: *const ::core::ffi::c_void) -> ::windows::core::Result<()> {
//         todo!()
//     }
// }

fn compile_shader_blob(
    filepath: &String,
    target: PCSTR,
    defines: &HashMap<String, String>,
) -> std::result::Result<ID3DBlob, String> {
    let mut macros = Vec::new();
    for item in defines {
        macros.push(D3D_SHADER_MACRO {
            Name: PCSTR(item.0.as_ptr()),
            Definition: PCSTR(item.1.as_ptr()),
        });
    }
    macros.push(D3D_SHADER_MACRO {
        Name: PCSTR::null(),
        Definition: PCSTR::null(),
    });

    let fullpath: HSTRING = (String::from("./shaders/") + filepath).into();
    let compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL1;

    let mut shader_blob = None;
    let mut error_blob = None;

    let _result = unsafe {
        D3DCompileFromFile(
            &fullpath,
            Some(macros.as_ptr()),
            // ID3DInclude::new(&D3DIncludeHandler {}),
            None,
            s!("main"),
            target,
            compile_flags,
            0,
            &mut shader_blob,
            Some(&mut error_blob),
        )
    };

    return match shader_blob {
        None => {
            let msg = extract_compile_error(error_blob);
            println!("Failed to compile shader file [{}]: [{}]", filepath, msg);
            return Err(msg);
        }
        Some(a) => Ok(a),
    };
}

pub struct D3DPixelShader {
    blob: ID3DBlob,
}

impl D3DPixelShader {
    pub fn compile<S: Into<String>>(filepath: S) -> std::result::Result<D3DPixelShader, String> {
        let blob = compile_shader_blob(&filepath.into(), s!("ps_5_1"), &HashMap::new())?;
        return Ok(D3DPixelShader { blob });
    }

    pub(crate) fn get_bytecode(&self) -> D3D12_SHADER_BYTECODE {
        todo!()
    }
}

pub struct D3DVertexShader {
    blob: ID3DBlob,
    ied: [D3D12_INPUT_ELEMENT_DESC; 3],
}

impl D3DVertexShader {
    pub fn compile<S: Into<String>>(filepath: S) -> std::result::Result<D3DVertexShader, String> {
        let blob = compile_shader_blob(&filepath.into(), s!("vs_5_1"), &HashMap::new())?;
        let ied = [
            D3D12_INPUT_ELEMENT_DESC {
                SemanticName: s!("POSITION"),
                SemanticIndex: 0,
                Format: DXGI_FORMAT_R32G32B32_FLOAT,
                InputSlot: 0,
                AlignedByteOffset: D3D12_APPEND_ALIGNED_ELEMENT,
                InputSlotClass: D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                InstanceDataStepRate: 0,
            },
            D3D12_INPUT_ELEMENT_DESC {
                SemanticName: s!("NORMAL"),
                SemanticIndex: 0,
                Format: DXGI_FORMAT_R32G32B32_FLOAT,
                InputSlot: 0,
                AlignedByteOffset: D3D12_APPEND_ALIGNED_ELEMENT,
                InputSlotClass: D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                InstanceDataStepRate: 0,
            },
            D3D12_INPUT_ELEMENT_DESC {
                SemanticName: s!("TEXCOORD"),
                SemanticIndex: 0,
                Format: DXGI_FORMAT_R32G32_FLOAT,
                InputSlot: 0,
                AlignedByteOffset: D3D12_APPEND_ALIGNED_ELEMENT,
                InputSlotClass: D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                InstanceDataStepRate: 0,
            },
        ];
        return Ok(D3DVertexShader { blob, ied });
    }

    pub(crate) fn get_bytecode(&self) -> D3D12_SHADER_BYTECODE {
        todo!()
    }

    pub(crate) fn get_input_layout(&self) -> D3D12_INPUT_LAYOUT_DESC {
        todo!()
    }
}
