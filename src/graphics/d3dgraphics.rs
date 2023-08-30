use crate::{graphics::*, material::PixelShader};
use arrayvec::ArrayVec;
use glam::{Mat4, Vec3, Vec3A};
use std::{cell::Cell, mem::{ManuallyDrop, size_of}};

use windows::{
    core::*, Win32::Foundation::*, Win32::Graphics::Direct3D::*, Win32::Graphics::Direct3D12::*,
    Win32::Graphics::Dxgi::Common::*, Win32::Graphics::Dxgi::*, Win32::System::Threading::*,
};

struct D3DCommandQueue {
    next_signal: Cell<u64>,
    last_completed: Cell<u64>,
    fence: ID3D12Fence,
    command_queue: ID3D12CommandQueue,
    event: HANDLE,
}

impl D3DCommandQueue {
    fn new(
        device: &ID3D12Device2,
        kind: D3D12_COMMAND_LIST_TYPE,
        priority: D3D12_COMMAND_QUEUE_PRIORITY,
    ) -> Result<D3DCommandQueue> {
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
            next_signal: Cell::new(16),
            last_completed: Cell::new(0),
            fence,
            command_queue,
            event,
        });
    }

    pub fn execute_command_list(&self, command_list: &ID3D12GraphicsCommandList) {
        unsafe { command_list.Close() }.unwrap();

        let command_list = Some(command_list.can_clone_into());
        unsafe { self.command_queue.ExecuteCommandLists(&[command_list]) }
    }

    pub fn signal(&self) -> u64 {
        let signal = self.next_signal.get();
        self.next_signal.replace(signal + 1);

        unsafe { self.command_queue.Signal(&self.fence, signal) }.unwrap();
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

        unsafe {
            self.fence.SetEventOnCompletion(signal, self.event).unwrap();
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

pub struct D3DGraphics {
    adapter: IDXGIAdapter4,
    device: ID3D12Device2,

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

    return Ok(String::from_utf16_lossy(
        &desc.Description[0..description_len],
    ));
}

fn create_device(adapter: &IDXGIAdapter4) -> Result<ID3D12Device2> {
    println!(
        "Initialising graphics device [{}]",
        get_adapter_description(adapter).unwrap_or_default()
    );
    let mut device: Option<ID3D12Device2> = None;
    unsafe { D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, &mut device)? }

    return Ok(device.unwrap());
}

fn create_command_resource(
    device: &ID3D12Device2,
    kind: D3D12_COMMAND_LIST_TYPE,
) -> Result<(
    ID3D12GraphicsCommandList,
    ID3D12CommandAllocator,
    D3DCommandQueue,
)> {
    let allocator = unsafe { device.CreateCommandAllocator(kind)? };

    let list: ID3D12GraphicsCommandList =
        unsafe { device.CreateCommandList(0, kind, &allocator, None)? };
    unsafe { list.Close() }?;

    let queue = D3DCommandQueue::new(device, kind, D3D12_COMMAND_QUEUE_PRIORITY_HIGH)?;

    return Ok((list, allocator, queue));
}

fn create_descriptor_heap(
    device: &ID3D12Device2,
    kind: D3D12_DESCRIPTOR_HEAP_TYPE,
    num_descriptors: u32,
) -> Result<ID3D12DescriptorHeap> {
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
    return unsafe {
        factory6.CreateSwapChainForHwnd(&queue.command_queue, *hwnd, &desc, None, None)?
    }
    .cast();
}

impl D3DGraphics {
    pub fn new() -> Result<D3DGraphics> {
        let adapter = create_adapter()?;
        let device = create_device(&adapter)?;

        let compute_commands = create_command_resource(&device, D3D12_COMMAND_LIST_TYPE_COMPUTE)?;

        return Ok(D3DGraphics {
            adapter,
            device,
            compute_command_list: compute_commands.0,
            compute_command_allocator: compute_commands.1,
            compute_command_queue: compute_commands.2,
        });
    }
}

#[derive(Debug, Clone)]
struct D3DResource {
    resource: ID3D12Resource,
    cpu_handle: D3D12_CPU_DESCRIPTOR_HANDLE,
}

const NUM_BUFFERS: usize = 3;
pub struct WindowRenderTarget {
    swap_chain: IDXGISwapChain4,
    command_queue: D3DCommandQueue,
    rtv_heap: ID3D12DescriptorHeap,
    dsv_heap: ID3D12DescriptorHeap,

    current_buffer: usize,
    frame_signals: [u64; NUM_BUFFERS],

    width: u32,
    height: u32,

    target_views: [D3DResource; NUM_BUFFERS],
    depth_stencil_view: D3DResource,
}

impl WindowRenderTarget {
    fn new(
        device: &ID3D12Device2,
        swap_chain: IDXGISwapChain4,
        command_queue: D3DCommandQueue,
    ) -> Result<WindowRenderTarget> {
        unsafe { swap_chain.ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0) }.unwrap();

        let mut swap_chain_desc: DXGI_SWAP_CHAIN_DESC = Default::default();
        unsafe { swap_chain.GetDesc(&mut swap_chain_desc) }.unwrap();
        let width = swap_chain_desc.BufferDesc.Width;
        let height = swap_chain_desc.BufferDesc.Height;
        println!("Initialising window resolution to [{}x{}]", width, height);

        let rtv_descriptor_heap =
            create_descriptor_heap(&device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3)?;
        let dsv_descriptor_heap =
            create_descriptor_heap(&device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1)?;

        let target_views =
            WindowRenderTarget::create_target_views(device, &swap_chain, &rtv_descriptor_heap);
        let depth_stencil_view = WindowRenderTarget::create_depth_stencil_resource(
            device,
            &dsv_descriptor_heap,
            width,
            height,
        );

        return Ok(WindowRenderTarget {
            swap_chain: swap_chain,
            command_queue: command_queue,
            rtv_heap: rtv_descriptor_heap,
            dsv_heap: dsv_descriptor_heap,

            current_buffer: 0,
            frame_signals: [0; NUM_BUFFERS],
            width: width,
            height: height,
            target_views: target_views,
            depth_stencil_view: depth_stencil_view,
        });
    }

    fn pre_render(&mut self, command_list: &ID3D12GraphicsCommandList) {
        self.current_buffer = unsafe { self.swap_chain.GetCurrentBackBufferIndex() } as usize;
        self.command_queue
            .wait_for_signal(self.frame_signals[self.current_buffer]);

        self.clear_resources(command_list);
    }

    fn clear_resources(&self, command_list: &ID3D12GraphicsCommandList) {
        let rtv = self.target_views[self.current_buffer].clone();
        let barrier = D3D12_RESOURCE_BARRIER {
            Type: D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            Flags: D3D12_RESOURCE_BARRIER_FLAG_NONE,
            Anonymous: D3D12_RESOURCE_BARRIER_0 {
                Transition: ManuallyDrop::new(D3D12_RESOURCE_TRANSITION_BARRIER {
                    pResource: ManuallyDrop::new(Some(rtv.resource)),
                    Subresource: D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    StateBefore: D3D12_RESOURCE_STATE_PRESENT,
                    StateAfter: D3D12_RESOURCE_STATE_RENDER_TARGET,
                }),
            }
            .abi(),
        };
        unsafe {
            command_list.ResourceBarrier(&[barrier]);
        }

        unsafe {
            const COLOUR: [f32; 4] = [0.0, 0.0, 0.0, 1.0];
            command_list.ClearRenderTargetView(rtv.cpu_handle, COLOUR.as_ptr(), None)
        }

        let dsv = self.depth_stencil_view.clone();
        unsafe {
            command_list.ClearDepthStencilView(dsv.cpu_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0, 0, &[]);
        }

        unsafe {
            command_list.OMSetRenderTargets(1, Some(&rtv.cpu_handle), false, Some(&dsv.cpu_handle));
        }

        let viewport = D3D12_VIEWPORT {
            Width: self.width as f32,
            Height: self.height as f32,
            MinDepth: 0.0,
            MaxDepth: 1.0,
            TopLeftX: 0.0,
            TopLeftY: 0.0,
        };
        unsafe {
            command_list.RSSetViewports(&[viewport]);
        }
    }

    fn present(&mut self, command_list: &ID3D12GraphicsCommandList) {
        let rtv = self.target_views[self.current_buffer].clone();
        let barrier = D3D12_RESOURCE_BARRIER {
            Type: D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            Flags: D3D12_RESOURCE_BARRIER_FLAG_NONE,
            Anonymous: D3D12_RESOURCE_BARRIER_0 {
                Transition: ManuallyDrop::new(D3D12_RESOURCE_TRANSITION_BARRIER {
                    pResource: ManuallyDrop::new(Some(rtv.resource)),
                    Subresource: D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    StateBefore: D3D12_RESOURCE_STATE_RENDER_TARGET,
                    StateAfter: D3D12_RESOURCE_STATE_PRESENT,
                }),
            }
            .abi(),
        };
        unsafe {
            command_list.ResourceBarrier(&[barrier]);
        }

        self.command_queue.execute_command_list(command_list);

        let sync_interval: u32 = 1;
        let present_flags: u32 = 0;
        unsafe { self.swap_chain.Present(sync_interval, present_flags) }.unwrap();

        self.frame_signals[self.current_buffer] = self.command_queue.signal();
    }

    fn update_window_size(&mut self, device: &ID3D12Device2) {
        //dealloc buffers by hand?

        unsafe {
            self.swap_chain
                .ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0)
        }
        .unwrap();

        let mut swap_chain_desc: DXGI_SWAP_CHAIN_DESC = Default::default();
        unsafe { self.swap_chain.GetDesc(&mut swap_chain_desc) }.unwrap();
        self.width = swap_chain_desc.BufferDesc.Width;
        self.height = swap_chain_desc.BufferDesc.Height;
        println!(
            "Updating window resolution to [{}x{}]",
            self.width, self.height
        );

        self.target_views =
            WindowRenderTarget::create_target_views(device, &self.swap_chain, &self.rtv_heap);
        self.depth_stencil_view = WindowRenderTarget::create_depth_stencil_resource(
            device,
            &self.dsv_heap,
            self.width,
            self.height,
        );
        self.current_buffer = unsafe { self.swap_chain.GetCurrentBackBufferIndex() } as usize;
    }

    fn create_target_views(
        device: &ID3D12Device2,
        swap_chain: &IDXGISwapChain4,
        rtv_heap: &ID3D12DescriptorHeap,
    ) -> [D3DResource; NUM_BUFFERS] {
        let rtv_descriptor_size =
            unsafe { device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) }
                as usize;
        let mut rtv_handle: D3D12_CPU_DESCRIPTOR_HANDLE =
            unsafe { rtv_heap.GetCPUDescriptorHandleForHeapStart() };

        let mut target_views = ArrayVec::<D3DResource, NUM_BUFFERS>::new();
        for i in 0..NUM_BUFFERS {
            let back_buffer: ID3D12Resource = unsafe { swap_chain.GetBuffer(i as u32) }.unwrap();

            unsafe { device.CreateRenderTargetView(&back_buffer, None, rtv_handle) };

            target_views.push(D3DResource {
                resource: back_buffer,
                cpu_handle: rtv_handle,
            });

            rtv_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                ptr: rtv_handle.ptr + rtv_descriptor_size,
            };
        }

        return target_views.into_inner().unwrap();
    }

    fn create_depth_stencil_resource(
        device: &ID3D12Device2,
        dsv_heap: &ID3D12DescriptorHeap,
        width: u32,
        height: u32,
    ) -> D3DResource {
        let clear_value: D3D12_CLEAR_VALUE = D3D12_CLEAR_VALUE {
            Format: DXGI_FORMAT_D32_FLOAT,
            Anonymous: D3D12_CLEAR_VALUE_0 {
                DepthStencil: D3D12_DEPTH_STENCIL_VALUE {
                    Depth: 1.0,
                    Stencil: 0,
                },
            },
        };

        // Update the depth-stencil view.
        let desc = D3D12_RESOURCE_DESC {
            Dimension: D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            Alignment: 0,
            Width: width as u64,
            Height: height,
            DepthOrArraySize: 1,
            MipLevels: 0,
            Format: DXGI_FORMAT_D32_FLOAT,
            SampleDesc: DXGI_SAMPLE_DESC {
                Count: 1,
                Quality: 0,
            },
            Layout: D3D12_TEXTURE_LAYOUT_UNKNOWN,
            Flags: D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
                | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
        };

        let mut depth_buffer_opt: Option<ID3D12Resource> = None;
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
                &mut depth_buffer_opt,
            )
        }
        .unwrap();
        let depth_buffer: ID3D12Resource =
            depth_buffer_opt.expect("failed to allocate depth buffer resource");

        let cpu_handle = unsafe { dsv_heap.GetCPUDescriptorHandleForHeapStart() };
        let dsv = D3D12_DEPTH_STENCIL_VIEW_DESC {
            Format: DXGI_FORMAT_D32_FLOAT,
            ViewDimension: D3D12_DSV_DIMENSION_TEXTURE2D,
            Flags: D3D12_DSV_FLAG_NONE,
            Anonymous: D3D12_DEPTH_STENCIL_VIEW_DESC_0 {
                Texture2D: D3D12_TEX2D_DSV { MipSlice: 0 },
            },
        };
        unsafe {
            device.CreateDepthStencilView(&depth_buffer, Some(&dsv), cpu_handle);
        }

        return D3DResource {
            resource: depth_buffer,
            cpu_handle: cpu_handle,
        };
    }
}

#[repr(C)]
struct D3DSlowVSConstants {
 world: Mat4,
 view: Mat4,
}

impl D3DSlowVSConstants {
    pub const SIZE_32_BIT:u32 = (size_of::<Self>() / size_of::<u32>()) as u32;
}

#[repr(C)]
struct D3DFastConstants {
model: Mat4,
sun_dir: Vec3A,
sun_sky_strength: f32,
sun_col: Vec3A
}

impl D3DFastConstants {
    pub const SIZE_32_BIT:u32 = (size_of::<Self>() / size_of::<u32>()) as u32;
}

pub struct D3DRootSignature {
    root_signature: ID3D12RootSignature
}

impl D3DRootSignature {
    fn check_root_signature_features(device: &ID3D12Device2) -> D3D12_FEATURE_DATA_ROOT_SIGNATURE {
        let mut feature_data = D3D12_FEATURE_DATA_ROOT_SIGNATURE {
            HighestVersion: D3D_ROOT_SIGNATURE_VERSION_1_1
        };
        
        let result = unsafe { 
            let ptr = &mut feature_data as *mut _ as *mut std::ffi::c_void;
            let size = size_of::<D3D12_FEATURE_DATA_ROOT_SIGNATURE>() as u32;
            device.CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, ptr, size)
        };
        if result.is_err()
        {
            feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }
        return feature_data;
    }

    fn init_as_constants(num_32bit_values: u32, 
        shader_register: u32,
        register_space: u32,
        visibility: D3D12_SHADER_VISIBILITY) -> D3D12_ROOT_PARAMETER1 {
        let constants = D3D12_ROOT_CONSTANTS { 
            ShaderRegister: shader_register, 
            RegisterSpace: register_space,
            Num32BitValues: num_32bit_values
        };
        return D3D12_ROOT_PARAMETER1 {
            ParameterType: D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
            Anonymous: D3D12_ROOT_PARAMETER1_0 {
                Constants: constants
            },
            ShaderVisibility: visibility
        };
    }

    fn init_1_1(params: Vec<D3D12_ROOT_PARAMETER1>,
         samplers: Vec<D3D12_STATIC_SAMPLER_DESC>,
          flags: D3D12_ROOT_SIGNATURE_FLAGS)
           -> D3D12_VERSIONED_ROOT_SIGNATURE_DESC {
        let desc = D3D12_ROOT_SIGNATURE_DESC1 {
            NumParameters: params.len() as u32,
            pParameters: params.as_ptr(),
            NumStaticSamplers: samplers.len() as u32,
            pStaticSamplers: samplers.as_ptr(),
            Flags: flags
        };

        return D3D12_VERSIONED_ROOT_SIGNATURE_DESC {
             Version: D3D_ROOT_SIGNATURE_VERSION_1_1, 
             Anonymous: D3D12_VERSIONED_ROOT_SIGNATURE_DESC_0 { Desc_1_1: desc }
            };
    }

    fn from_pixel_shader(pixel_shader: &PixelShader, device: &ID3D12Device2) -> D3DRootSignature {
        let feature_data = Self::check_root_signature_features(device);
    
        let mut params:Vec<D3D12_ROOT_PARAMETER1> = Vec::new();
        params.push(Self::init_as_constants(D3DSlowVSConstants::SIZE_32_BIT, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX));
        params.push(Self::init_as_constants(D3DFastConstants::SIZE_32_BIT, 1, 0, D3D12_SHADER_VISIBILITY_ALL));
    
        let inputs = pixel_shader.inputs;
        if !inputs.is_empty()
        {   
            // uint32_t t = static_cast<uint32_t>(num_inputs);
            // CD3DX12_DESCRIPTOR_RANGE1 desc_range{D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV, t, 0};
            // params[2].InitAsDescriptorTable(1U, &desc_range, D3D12_SHADER_VISIBILITY_PIXEL);
        }
    
        // create a static sampler
        let sampler = D3D12_STATIC_SAMPLER_DESC{
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
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
        

        let desc = Self::init_1_1(params, vec![sampler], flags);
        
        let mut blob = None;
        let mut error_blob = None;
        unsafe {
            D3D12SerializeVersionedRootSignature(&desc, &mut blob, &mut error_blob);
        }
        let root_signature = unsafe { device.CreateRootSignature(0, &blob) }.unwrap();
    
        return D3DRootSignature { root_signature: root_signature };
    }
}

pub struct D3DRenderer<'a> {
    graphics: &'a D3DGraphics,

    draw_command_list: ID3D12GraphicsCommandList,
    draw_command_allocator: ID3D12CommandAllocator,

    render_target: WindowRenderTarget,
}

impl<'a> Renderer for D3DRenderer<'a> {
    fn apply(&mut self, items: RenderQueueItems) {
        for instance in items.new_meshes {
            let d3d_mesh = self.graphics.load_mesh(instance.mesh);
            let d3d_material = self.graphics.load_material(instance.material);

            let ps = instance.material.shader;
            let root_signature = D3DRootSignature::from_pixel_shader(ps, &self.graphics.device);
        }
    }

    fn render_frame(&mut self) {
        self.pre_render();
        self.present();
    }
}

impl<'a> D3DRenderer<'a> {
    fn pre_render(&mut self) {
        unsafe { self.draw_command_allocator.Reset() }.unwrap();
        unsafe {
            self.draw_command_list
                .Reset(&self.draw_command_allocator, None)
        }
        .unwrap();

        self.render_target.pre_render(&self.draw_command_list);
    }

    fn present(&mut self) {
        self.render_target.present(&self.draw_command_list);
    }
}

impl<'a> CreateRenderer<'a> for D3DGraphics {
    type Canvas = HWND;
    type Output = D3DRenderer<'a>;
    type Err = Error;

    fn create_renderer(&'a self, canvas: &HWND) -> Result<D3DRenderer<'a>> {
        let draw_commands = create_command_resource(&self.device, D3D12_COMMAND_LIST_TYPE_DIRECT)?;

        let swap_chain = create_swap_chain(&draw_commands.2, canvas)?;
        let render_target: WindowRenderTarget =
            WindowRenderTarget::new(&self.device, swap_chain, draw_commands.2).unwrap();

        return Ok(D3DRenderer {
            graphics: self,
            draw_command_list: draw_commands.0,
            draw_command_allocator: draw_commands.1,
            render_target,
        });
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_get_adapter_description() {
        let adapter = create_adapter();
        let desc = get_adapter_description(&adapter.unwrap());

        assert!(desc.is_ok());
    }

    #[test]
    fn test_create_device() {
        let adapter = create_adapter();
        let device = create_device(&adapter.unwrap());

        assert!(device.is_ok());
    }

    #[test]
    fn test_create_command_resource() {
        let adapter = create_adapter();
        let device = create_device(&adapter.unwrap());
        let commands = create_command_resource(&device.unwrap(), D3D12_COMMAND_LIST_TYPE_DIRECT);

        assert!(commands.is_ok());
    }

    #[test]
    fn test_create_descriptor_heap() {
        let adapter = create_adapter();
        let device = create_device(&adapter.unwrap());
        let heap = create_descriptor_heap(&device.unwrap(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3);

        assert!(heap.is_ok());
    }
}
