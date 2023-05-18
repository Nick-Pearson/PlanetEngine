
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

pub struct D3DRenderer {
    adapter: IDXGIAdapter4,
    device: ID3D12Device2,
    // swap_chain: IDXGISwapChain4,

    // // class D3DCommandQueue* draw_command_queue_ = nullptr;
    draw_command_list: ID3D12GraphicsCommandList,
    draw_command_allocator: ID3D12CommandAllocator,

    // // class D3DCommandQueue* compute_command_queue_ = nullptr;
    compute_command_list: ID3D12GraphicsCommandList,
    compute_command_allocator: ID3D12CommandAllocator,

    rtv_descriptor_heap: ID3D12DescriptorHeap,
    dsv_descriptor_heap: ID3D12DescriptorHeap,
}

fn create_adapter() -> Result<IDXGIAdapter4> {
    let factory6: IDXGIFactory6 = unsafe { CreateDXGIFactory1() }.unwrap();

    return unsafe { factory6.EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE) };
}

fn get_adapter_description(adapter: &IDXGIAdapter4) -> Result<String> {
    let mut desc = DXGI_ADAPTER_DESC1::default();
    unsafe { adapter.GetDesc1(&mut desc)? }

    let description_len = desc.Description.iter().position(|&r| r == 0).unwrap();

    return Ok(String::from_utf16_lossy(&desc.Description[0 .. description_len]));
}

fn create_device(adapter: &IDXGIAdapter4) -> Result<ID3D12Device2> {
    println!("Initialising graphics device [{}]", get_adapter_description(adapter).unwrap_or_default());
    let mut device: Option<ID3D12Device2> = None;
    unsafe { D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, &mut device)? }

    return Ok(device.unwrap());
}

fn create_command_resource(device: &ID3D12Device2, kind: D3D12_COMMAND_LIST_TYPE) -> Result<(ID3D12GraphicsCommandList, ID3D12CommandAllocator)> {
    let allocator = unsafe { device.CreateCommandAllocator(kind)? };
    
    let list:ID3D12GraphicsCommandList = unsafe { device.CreateCommandList(0, kind, &allocator, None)? };
    unsafe { list.Close() }?;

    return Ok((list, allocator));
}

fn create_descriptor_heap(device: &ID3D12Device2, kind: D3D12_DESCRIPTOR_HEAP_TYPE, num_descriptors: u32) -> Result<ID3D12DescriptorHeap> {
    let desc = D3D12_DESCRIPTOR_HEAP_DESC {
        Type: kind,
        NumDescriptors: num_descriptors,
        Flags: D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        NodeMask: 0,
    };
    return unsafe { device.CreateDescriptorHeap(&desc) };
}

impl D3DRenderer {
    fn new() -> Result<D3DRenderer> {
        let adapter = create_adapter()?;
        let device = create_device(&adapter)?;

        let draw_commands = create_command_resource(&device, D3D12_COMMAND_LIST_TYPE_DIRECT)?;
        let compute_commands = create_command_resource(&device, D3D12_COMMAND_LIST_TYPE_COMPUTE)?;

        let rtv_descriptor_heap = create_descriptor_heap(&device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3)?;
        let dsv_descriptor_heap = create_descriptor_heap(&device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1)?;

        return Ok(D3DRenderer {
            adapter,
            device,
            draw_command_list: draw_commands.0,
            draw_command_allocator: draw_commands.1,
            compute_command_list: compute_commands.0,
            compute_command_allocator: compute_commands.1,
            rtv_descriptor_heap,
            dsv_descriptor_heap,
        });
    }
}

impl Renderer for D3DRenderer
{
    fn render_frame(&self) {
    }
}

pub fn new_renderer() -> D3DRenderer
{
    return D3DRenderer::new().unwrap();
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_get_adapter_description() 
    {
        let adapter = create_adapter();
        let desc = get_adapter_description(&adapter.unwrap());

        assert!(desc.is_ok());
    }

    #[test]
    fn test_create_device() 
    {
        let adapter = create_adapter();
        let device = create_device(&adapter.unwrap());

        assert!(device.is_ok());
    }

    #[test]
    fn test_create_command_resource() 
    {
        let adapter = create_adapter();
        let device = create_device(&adapter.unwrap());
        let commands = create_command_resource(&device.unwrap(), D3D12_COMMAND_LIST_TYPE_DIRECT);

        assert!(commands.is_ok());
    }

    #[test]
    fn test_create_descriptor_heap() 
    {
        let adapter = create_adapter();
        let device = create_device(&adapter.unwrap());
        let heap = create_descriptor_heap(&device.unwrap(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3);

        assert!(heap.is_ok());
    }

    
}