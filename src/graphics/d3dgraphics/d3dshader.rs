use std::{
    alloc::Layout,
    alloc::{alloc_zeroed, dealloc},
    collections::HashMap,
    ffi::{c_void, CStr},
    fs::File,
    io::Read,
    path::PathBuf,
};

use log::warn;
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

#[allow(overflowing_literals)]
pub const E_FAIL: HRESULT = HRESULT(0x80004005_i32);
#[allow(overflowing_literals)]
pub const E_NOTIMPL: HRESULT = HRESULT(0x80004001_i32);

struct D3DIncludeHandler {
    base_directory: PathBuf,
}

impl D3DIncludeHandler {
    const BUFFER_SIZE: usize = 1024 * 1024;
    const MEM_LAYOUT: Layout = Layout::new::<[u8; Self::BUFFER_SIZE]>();

    fn new<S: Into<String>>(shader_path: S) -> D3DIncludeHandler {
        let mut dir = PathBuf::from(shader_path.into());
        dir.pop();
        D3DIncludeHandler {
            base_directory: dir,
        }
    }
}

#[allow(non_snake_case)]
impl ID3DInclude_Impl for D3DIncludeHandler {
    fn Open(
        &self,
        includetype: D3D_INCLUDE_TYPE,
        pfilename: &::windows::core::PCSTR,
        _pparentdata: *const c_void,
        ppdata: *mut *mut c_void,
        pbytes: *mut u32,
    ) -> ::windows::core::Result<()> {
        if let D3D_INCLUDE_LOCAL = includetype {
            let filename = unsafe { pfilename.to_string() }.unwrap_or_default();
            let mut path = self.base_directory.clone();
            path.push(&filename);

            let data_result = File::open(path.as_path());
            match data_result {
                Ok(mut file) => {
                    let file_size = file.metadata().unwrap().len();
                    if file_size > Self::BUFFER_SIZE as u64 {
                        return Err(Error::new(E_FAIL, "File size too large".into()));
                    }

                    let ptr = unsafe { alloc_zeroed(Self::MEM_LAYOUT) };
                    let slice = unsafe { std::slice::from_raw_parts_mut(ptr, Self::BUFFER_SIZE) };
                    let result = file.read(slice);

                    match result {
                        Ok(_) => {
                            unsafe {
                                *ppdata = ptr as *mut c_void;
                                *pbytes = file_size as u32;
                            }
                            Ok(())
                        }
                        Err(_) => Err(Error::new(E_FAIL, "Failed to read from file".into())),
                    }
                }
                Err(_error) => Err(Error::new(E_FAIL, "Failed to open file".into())),
            }
        } else {
            Err(Error::new(E_NOTIMPL, "Unsupported include type".into()))
        }
    }

    fn Close(&self, pdata: *const c_void) -> ::windows::core::Result<()> {
        unsafe { dealloc(pdata as *mut u8, Self::MEM_LAYOUT) };
        Ok(())
    }
}

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

    let fullpath = String::from("./shaders/") + filepath;
    let compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL1;
    let include_handler = D3DIncludeHandler::new(&fullpath);
    let include_interface = ID3DInclude::new(&include_handler);

    let mut shader_blob = None;
    let mut error_blob = None;

    let _result = unsafe {
        D3DCompileFromFile(
            &fullpath.into(),
            Some(macros.as_ptr()),
            &*include_interface,
            s!("main"),
            target,
            compile_flags,
            0,
            &mut shader_blob,
            Some(&mut error_blob),
        )
    };

    match shader_blob {
        None => {
            let msg = extract_compile_error(error_blob);
            warn!("Failed to compile shader file [{}]: [{}]", filepath, msg);
            Err(msg)
        }
        Some(a) => Ok(a),
    }
}

fn get_bytecode(blob: &ID3DBlob) -> D3D12_SHADER_BYTECODE {
    unsafe {
        D3D12_SHADER_BYTECODE {
            pShaderBytecode: blob.GetBufferPointer(),
            BytecodeLength: blob.GetBufferSize(),
        }
    }
}

pub struct D3DPixelShader {
    blob: ID3DBlob,
}

impl<'a> D3DPixelShader {
    pub fn compile<S: Into<String>>(filepath: S) -> std::result::Result<D3DPixelShader, String> {
        let blob = compile_shader_blob(&filepath.into(), s!("ps_5_1"), &HashMap::new())?;
        Ok(D3DPixelShader { blob })
    }

    pub(crate) fn get_bytecode(&'a self) -> D3D12_SHADER_BYTECODE {
        get_bytecode(&self.blob)
    }
}

pub struct D3DVertexShader {
    blob: ID3DBlob,
    ied: [D3D12_INPUT_ELEMENT_DESC; 3],
}

impl<'a> D3DVertexShader {
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
        Ok(D3DVertexShader { blob, ied })
    }

    pub(crate) fn get_bytecode(&'a self) -> D3D12_SHADER_BYTECODE {
        get_bytecode(&self.blob)
    }

    pub(crate) fn get_input_layout(&'a self) -> D3D12_INPUT_LAYOUT_DESC {
        D3D12_INPUT_LAYOUT_DESC {
            pInputElementDescs: self.ied.as_ptr(),
            NumElements: self.ied.len() as u32,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn all_pixel_shaders_compile() {
        let _ = D3DPixelShader::compile("ps/FallbackShader.hlsl");
        let _ = D3DPixelShader::compile("ps/PixelShader.hlsl");
        let _ = D3DPixelShader::compile("ps/SkySphere.hlsl");
        let _ = D3DPixelShader::compile("ps/TexturedShader.hlsl");
        let _ = D3DPixelShader::compile("ps/TreeShader.hlsl");
        let _ = D3DPixelShader::compile("ps/WireframeShader.hlsl");
    }

    #[test]
    fn all_vertex_shaders_compile() {
        let _ = D3DPixelShader::compile("vs/VertexShader.hlsl");
    }
}
