
use crate::renderer::Renderer;

use windows::{
    core::*,
    Win32::Foundation::*,
    Win32::Graphics::Direct3D::Fxc::*,
    Win32::Graphics::Direct3D::*,
    Win32::Graphics::Direct3D12::*,
    Win32::Graphics::Dxgi::Common::*,
    Win32::Graphics::Dxgi::*,
};

pub struct D3DRenderer
{
    adapter: IDXGIAdapter4,
    device: ID3D12Device2,
    // swap_chain: IDXGISwapChain4,

    // // class D3DCommandQueue* draw_command_queue_ = nullptr;
    // draw_command_list: ID3D12GraphicsCommandList,
    // draw_command_allocator: ID3D12CommandAllocator,

    // // class D3DCommandQueue* compute_command_queue_ = nullptr;
    // compute_command_list: ID3D12GraphicsCommandList,
    // compute_command_allocator: ID3D12CommandAllocator,

    // rtv_descriptor_heap: ID3D12DescriptorHeap,
    // dsv_descriptor_heap: ID3D12DescriptorHeap,
}

fn create_adapter() -> IDXGIAdapter4
{
    let factory6: IDXGIFactory6 = unsafe { CreateDXGIFactory1() }.unwrap();

    let adapter:IDXGIAdapter4 = unsafe { factory6.EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE) }
        .expect("No compatible graphics devices found");

    return adapter;
}

fn get_adapter_description(adapter: &IDXGIAdapter4) -> Result<String>
{
    let mut desc = DXGI_ADAPTER_DESC1::default();
    unsafe { adapter.GetDesc1(&mut desc)? }

    let description_len = desc.Description.iter().position(|&r| r == 0).unwrap();

    return Ok(String::from_utf16_lossy(&desc.Description[0 .. description_len]));
}

fn create_device(adapter: &IDXGIAdapter4) -> ID3D12Device2
{
    println!("Initialising graphics device [{}]", get_adapter_description(adapter).unwrap_or_default());
    let mut device: Option<ID3D12Device2> = None;
    unsafe { D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, &mut device) }
        .expect("unable to create device");

    return device.unwrap();
}

impl D3DRenderer {
    fn new() -> D3DRenderer {
        let adapter = create_adapter();
        let device = create_device(&adapter);

        return D3DRenderer {
            adapter, device
        };
    }
}

impl Renderer for D3DRenderer
{
    fn render_frame(&self) {
    }
}

pub fn new_renderer() -> D3DRenderer
{
    return D3DRenderer::new();
}