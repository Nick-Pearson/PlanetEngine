
use crate::{graphics::*, Window};
use std::{cell::Cell, convert::TryInto};

use windows::{
    core::*,
    Win32::Foundation::*,
    Win32::Graphics::Direct3D::*,
    Win32::Graphics::Direct3D12::*,
    Win32::Graphics::Dxgi::Common::*,
    Win32::Graphics::Dxgi::*,
    Win32::System::Threading::*,
};


struct D3DCommandQueue
{
    next_signal: u64,
    last_completed: Cell<u64>,
    fence: ID3D12Fence,
    command_queue: ID3D12CommandQueue,
    event: HANDLE,
}

impl D3DCommandQueue {
    fn new(device: &ID3D12Device2, kind: D3D12_COMMAND_LIST_TYPE, priority: D3D12_COMMAND_QUEUE_PRIORITY) -> Result<D3DCommandQueue> {
        let desc = D3D12_COMMAND_QUEUE_DESC {
            Priority: priority.0,
            Flags: D3D12_COMMAND_QUEUE_FLAG_NONE,
            Type: kind,
            ..Default::default()
        };

        let fence = unsafe { device.CreateFence(0, D3D12_FENCE_FLAG_NONE)? };
        let command_queue = unsafe { device.CreateCommandQueue(&desc)? };
        let event = unsafe { CreateEventW(None, false, false, None)? };

        return Ok(D3DCommandQueue {
            next_signal: 16,
            last_completed: Cell::new(0),
            fence,
            command_queue,
            event,
        });
    }

    pub fn execute_command_list(&self, command_list: &ID3D12GraphicsCommandList) {
        unsafe { command_list.Close() };

        let command_list = Some(command_list.can_clone_into());
        unsafe { self.command_queue.ExecuteCommandLists(&[command_list]) }
    }
    
    pub fn signal(&mut self) -> u64 {
        self.next_signal = self.next_signal + 1;
        let signal = self.next_signal;

        unsafe { self.command_queue.Signal(&self.fence, signal) };
        return signal;
    }
    
    pub fn update_and_get_last_completed_signal(&self) -> u64 {
        let updated = unsafe { self.fence.GetCompletedValue() };
        self.last_completed.replace(updated);
        return updated;
    }
    
    pub fn wait_for_signal(&self, signal: u64) {
        if self.last_completed.get() >= signal {
            return;
        }

        if self.update_and_get_last_completed_signal() >= signal {
            return;
        }
         
        unsafe{
            self.fence.SetEventOnCompletion(signal, self.event);
            WaitForSingleObjectEx(self.event, INFINITE, false);
        }
        self.last_completed.replace(signal);
    }
}

impl Drop for D3DCommandQueue {
    fn drop(&mut self) {
        unsafe { CloseHandle(self.event) };
    }
}

pub struct D3DGraphics 
{
    adapter: IDXGIAdapter4,
    device: ID3D12Device2,

    draw_command_queue: D3DCommandQueue,
    draw_command_list: ID3D12GraphicsCommandList,
    draw_command_allocator: ID3D12CommandAllocator,

    compute_command_queue: D3DCommandQueue,
    compute_command_list: ID3D12GraphicsCommandList,
    compute_command_allocator: ID3D12CommandAllocator,
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

fn create_command_resource(device: &ID3D12Device2, kind: D3D12_COMMAND_LIST_TYPE) -> Result<(ID3D12GraphicsCommandList, ID3D12CommandAllocator, D3DCommandQueue)> {
    let allocator = unsafe { device.CreateCommandAllocator(kind)? };
    
    let list:ID3D12GraphicsCommandList = unsafe { device.CreateCommandList(0, kind, &allocator, None)? };
    unsafe { list.Close() }?;

    let queue = D3DCommandQueue::new(device, kind, D3D12_COMMAND_QUEUE_PRIORITY_HIGH)?;

    return Ok((list, allocator, queue));
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

fn create_swap_chain(queue: &D3DCommandQueue, hwnd: &HWND) -> Result<IDXGISwapChain4> {
    let factory6: IDXGIFactory6 = unsafe { CreateDXGIFactory1() }.unwrap();

    let desc = DXGI_SWAP_CHAIN_DESC1 {
        Width: 1280,
        Height: 720,
        Format: DXGI_FORMAT_R8G8B8A8_UNORM,
        SampleDesc: DXGI_SAMPLE_DESC {
            Count: 1,
            ..Default::default()
        },
        BufferUsage: DXGI_USAGE_RENDER_TARGET_OUTPUT,
        BufferCount: 3,
        Scaling: DXGI_SCALING_STRETCH,
        SwapEffect: DXGI_SWAP_EFFECT_FLIP_DISCARD,
        AlphaMode: DXGI_ALPHA_MODE_UNSPECIFIED,
        ..Default::default()
    };
    return unsafe { factory6.CreateSwapChainForHwnd(&queue.command_queue, *hwnd, &desc, None, None)? }
        .cast();
}

impl D3DGraphics {
    pub fn new() -> Result<D3DGraphics> {
        let adapter = create_adapter()?;
        let device = create_device(&adapter)?;

        let draw_commands = create_command_resource(&device, D3D12_COMMAND_LIST_TYPE_DIRECT)?;
        let compute_commands = create_command_resource(&device, D3D12_COMMAND_LIST_TYPE_COMPUTE)?;

        return Ok(D3DGraphics {
            adapter,
            device,
            draw_command_list: draw_commands.0,
            draw_command_allocator: draw_commands.1,
            draw_command_queue: draw_commands.2,
            compute_command_list: compute_commands.0,
            compute_command_allocator: compute_commands.1,
            compute_command_queue: compute_commands.2
        });
    }
}


struct D3DResource {
    resource: ID3D12Resource,
    cpu_handle: D3D12_CPU_DESCRIPTOR_HANDLE
}

const NUM_BUFFERS:usize = 3;
pub struct WindowRenderTarget <'a>
{
    swap_chain: IDXGISwapChain4,
    command_queue: &'a D3DCommandQueue,
    rtv_heap: ID3D12DescriptorHeap,
    dsv_heap: ID3D12DescriptorHeap,
    
    current_buffer: usize,
    frame_signals: [u64; NUM_BUFFERS],

    width: u32,
    height: u32,

    depth_stencil_view: D3DResource,
}

impl<'a> WindowRenderTarget <'a> {
    fn new(device: &ID3D12Device2, swap_chain: IDXGISwapChain4, command_queue: &'a D3DCommandQueue) -> Result<WindowRenderTarget<'a>> {
        let rtv_descriptor_heap = create_descriptor_heap(&device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3)?;
        let dsv_descriptor_heap = create_descriptor_heap(&device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1)?;

        let depth_stencil_view = WindowRenderTarget::create_depth_stencil_resource(device, 0, 0);

        return Ok(WindowRenderTarget{
            swap_chain: swap_chain,
            command_queue: command_queue,
            rtv_heap: rtv_descriptor_heap,
            dsv_heap: dsv_descriptor_heap,

            current_buffer: 0,
            frame_signals: [0; NUM_BUFFERS],
            width:0,
            height:0,
            depth_stencil_view: depth_stencil_view
         });
    }

    fn pre_render(&mut self) {
        self.current_buffer = unsafe { self.swap_chain.GetCurrentBackBufferIndex() } as usize;
        self.command_queue.wait_for_signal(self.frame_signals[self.current_buffer]);
    }
    
    fn present(&mut self) {
        let sync_interval:u32 = 1;
        let present_flags:u32 = 0;
        unsafe { self.swap_chain.Present(sync_interval, present_flags) };

        self.frame_signals[self.current_buffer] = self.command_queue.signal();
    }

    fn update_window_size(&mut self, device: &ID3D12Device2) {
        for i in 0..NUM_BUFFERS {
            self.command_queue.wait_for_signal(self.frame_signals_[i]);

            if (self.target_view[i].resource_) {
                self.target_view[i].resource_->Release();
            }

            self.target_view_[i].resource_ = nullptr;
        }

        unsafe { self.swap_chain.ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0) };

        let swap_chain_desc: DXGI_SWAP_CHAIN_DESC;
        unsafe { self.swap_chain.GetDesc(&mut swap_chain_desc) };
        self.width = swap_chain_desc.BufferDesc.Width;
        self.height = swap_chain_desc.BufferDesc.Height;

        
        let rtv_descriptor_size = unsafe { device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)} as usize;
        let mut rtv_handle: D3D12_CPU_DESCRIPTOR_HANDLE = unsafe{ self.rtv_heap.GetCPUDescriptorHandleForHeapStart() };

        for i in 0..NUM_BUFFERS {
            let back_buffer:ID3D12Resource = unsafe { self.swap_chain.GetBuffer(i as u32) }
                .unwrap();

            unsafe { device.CreateRenderTargetView(&back_buffer, None, rtv_handle) };

            self.target_view_[i].resource_ = back_buffer;
            self.target_view_[i].cpu_handle_ = rtv_handle;

            rtv_handle = D3D12_CPU_DESCRIPTOR_HANDLE{ptr: rtv_handle.ptr + rtv_descriptor_size};
        }

        self.depth_stencil_view = WindowRenderTarget::create_depth_stencil_resource(device, self.width, self.height);        
        self.current_buffer = unsafe { self.swap_chain.GetCurrentBackBufferIndex() };
    }

    fn create_depth_stencil_resource(device: &ID3D12Device2, width: u32, height: u32) -> D3DResource {
        let clear_value: D3D12_CLEAR_VALUE = D3D12_CLEAR_VALUE{
            Format: DXGI_FORMAT_D32_FLOAT,
            Anonymous: D3D12_CLEAR_VALUE_0{DepthStencil: D3D12_DEPTH_STENCIL_VALUE{ Depth: 1.0, Stencil: 0 }}
        };

        // Update the depth-stencil view.
        let desc = D3D12_RESOURCE_DESC {
            Dimension: D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            Width: width as u64,
            Height: height,
            DepthOrArraySize: 1,
            MipLevels: 0,
            SampleDesc: DXGI_SAMPLE_DESC {
                Count: 1,
                Quality: 0,
            },
            Flags: D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
            ..Default::default()
        };

        let depth_buffer_opt: Option<ID3D12Resource>;
        unsafe { 
            device.CreateCommittedResource(
                &D3D12_HEAP_PROPERTIES {
                    Type: D3D12_HEAP_TYPE_DEFAULT,
                    ..Default::default()
                },
                D3D12_HEAP_FLAG_NONE, 
                &desc, 
                D3D12_RESOURCE_STATE_DEPTH_WRITE, 
                Some(&clear_value), 
                &mut depth_buffer_opt
            ) 
        };
        let depth_buffer: ID3D12Resource = depth_buffer_opt.expect("failed to allocate depth buffer resource");

        let cpu_handle = unsafe { self.dsv_heap.GetCPUDescriptorHandleForHeapStart() };
        let dsv = D3D12_DEPTH_STENCIL_VIEW_DESC{
            Format: DXGI_FORMAT_D32_FLOAT,
            ViewDimension: D3D12_DSV_DIMENSION_TEXTURE2D,
            Flags: D3D12_DSV_FLAG_NONE,
            Anonymous: D3D12_DEPTH_STENCIL_VIEW_DESC_0{Texture2D: D3D12_TEX2D_DSV{MipSlice: 0}}
        };
        unsafe {
            device.CreateDepthStencilView(
                &depth_buffer,
                Some(&dsv),
                cpu_handle
            );
        }

        return D3DResource {
            resource: depth_buffer,
            cpu_handle: cpu_handle
        };
    }

}

pub struct D3DRenderer<'a>
{
    render_target: WindowRenderTarget<'a>
}

impl<'a> Renderer for D3DRenderer<'a> {
    fn render_frame(&self) {

    }
}

impl<'a> CreateRenderer<'a> for D3DGraphics {
    type Canvas = HWND;
    type Output = D3DRenderer<'a>;
    type Err = Error;

    fn create_renderer(&'a self, canvas: &HWND) -> Result<D3DRenderer> {
        let swap_chain = create_swap_chain(&self.draw_command_queue, canvas)?;
        let render_target: WindowRenderTarget<'a> = WindowRenderTarget::new(&self.device, swap_chain, &self.draw_command_queue)
            .unwrap();

        return Ok(D3DRenderer{
            render_target
        });
    }
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