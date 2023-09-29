mod d3dcommandqueue;
mod d3dmesh;
mod d3dpipelinestate;
mod d3dresources;
mod d3drootsignature;
mod d3dshader;

use crate::graphics::*;
use arrayvec::ArrayVec;
use glam::Mat4;
use std::cmp::Ordering;
use std::ffi::c_void;
use windows::Win32::Storage::FileSystem::{
    FindClose, FindFirstFileW, FindNextFileW, FILE_ATTRIBUTE_DIRECTORY, FILE_FLAGS_AND_ATTRIBUTES,
    WIN32_FIND_DATAW,
};
use windows::Win32::System::LibraryLoader::{GetModuleHandleW, LoadLibraryW};
use windows::Win32::UI::Shell::{FOLDERID_ProgramFiles, SHGetKnownFolderPath, KF_FLAG_DEFAULT};

use std::mem::ManuallyDrop;

use windows::{
    core::*, Win32::Foundation::*, Win32::Graphics::Direct3D::*, Win32::Graphics::Direct3D12::*,
    Win32::Graphics::Dxgi::Common::*, Win32::Graphics::Dxgi::*,
};

use self::d3dcommandqueue::{create_command_resource, D3DCommandQueue};
use self::d3dmesh::D3DMesh;
use self::d3dpipelinestate::D3DPipelineState;
use self::d3dresources::D3DResources;
use self::d3drootsignature::{D3DFastConstants, D3DRootSignature, D3DSlowVSConstants};

pub struct D3DGraphics {
    adapter: IDXGIAdapter4,
    device: ID3D12Device2,

    resources: D3DResources,

    compute_command_queue: D3DCommandQueue,
    compute_command_list: ID3D12GraphicsCommandList,
    compute_command_allocator: ID3D12CommandAllocator,
    debug: ID3D12Debug,
}

fn create_adapter() -> Result<IDXGIAdapter4> {
    let flags = DXGI_CREATE_FACTORY_DEBUG;
    let factory6: IDXGIFactory6 = unsafe { CreateDXGIFactory2(flags) }.unwrap();

    unsafe { factory6.EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE) }
}

fn get_adapter_description(adapter: &IDXGIAdapter4) -> Result<String> {
    let mut desc = DXGI_ADAPTER_DESC1::default();
    unsafe { adapter.GetDesc1(&mut desc)? }

    let description_len = desc.Description.iter().position(|&r| r == 0).unwrap();

    Ok(String::from_utf16_lossy(
        &desc.Description[0..description_len],
    ))
}

fn create_device(adapter: &IDXGIAdapter4) -> Result<ID3D12Device2> {
    println!(
        "Initialising graphics device [{}]",
        get_adapter_description(adapter).unwrap_or_default()
    );
    let mut device: Option<ID3D12Device2> = None;
    unsafe { D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, &mut device)? }

    Ok(device.unwrap())
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
    unsafe { device.CreateDescriptorHeap(&desc) }
}

fn create_swap_chain(queue: &D3DCommandQueue, hwnd: &HWND) -> Result<IDXGISwapChain4> {
    let flags = DXGI_CREATE_FACTORY_DEBUG;
    let factory6: IDXGIFactory6 = unsafe { CreateDXGIFactory2(flags) }.unwrap();

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
    unsafe { factory6.CreateSwapChainForHwnd(&queue.command_queue, *hwnd, &desc, None, None)? }
        .cast()
}

fn wcscmp(a: &[u16], b: &[u16]) -> Ordering {
    for (ai, bi) in a.iter().zip(b.iter()) {
        match ai.cmp(bi) {
            Ordering::Equal => continue,
            ord => return ord,
        }
    }
    a.len().cmp(&b.len())
}

unsafe fn get_latest_win_pix_gpu_capturer_path() -> Option<String> {
    let program_files_path = SHGetKnownFolderPath(&FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, None);
    if program_files_path.is_err() {
        return None;
    }

    let pix_search_path = String::from("\\\\?\\")
        + &program_files_path.unwrap().to_string().unwrap()
        + &String::from("\\Microsoft PIX\\*");
    let bytes: Vec<u16> = pix_search_path.encode_utf16().collect();
    let ptr: PCWSTR = PCWSTR::from_raw(bytes.as_ptr());

    let mut find_data = WIN32_FIND_DATAW::default();

    let mut newest_version_found: Option<[u16; 260]> = None;
    let h_find = FindFirstFileW(ptr, &mut find_data);

    h_find
        .map(|handle| {
            loop {
                if FILE_ATTRIBUTE_DIRECTORY
                    .contains(FILE_FLAGS_AND_ATTRIBUTES(find_data.dwFileAttributes))
                    && find_data.cFileName[0] != '.' as u16
                    && newest_version_found.is_none()
                    && wcscmp(&newest_version_found.unwrap(), &find_data.cFileName)
                        == Ordering::Less
                {
                    newest_version_found = Some(find_data.cFileName);
                }

                if FindNextFileW(handle, &mut find_data) == BOOL(0) {
                    break;
                }
            }
            FindClose(handle);
            newest_version_found.map(|b| String::from_utf16_lossy(&b))
        })
        .unwrap_or(None)
}

impl D3DGraphics {
    pub fn new() -> Result<D3DGraphics> {
        let mut debug_opt: Option<ID3D12Debug> = None;
        unsafe { D3D12GetDebugInterface(&mut debug_opt) }.unwrap();
        let debug: ID3D12Debug = debug_opt.unwrap();
        unsafe { debug.EnableDebugLayer() };
        unsafe {
            let pix_module = GetModuleHandleW(w!("WinPixGpuCapturer.dll"));
            if pix_module.is_err() {
                match get_latest_win_pix_gpu_capturer_path() {
                    Some(path) => {
                        let bytes: Vec<u16> = path.encode_utf16().collect();
                        let ptr: PCWSTR = PCWSTR::from_raw(bytes.as_ptr());
                        LoadLibraryW(ptr).unwrap();
                    }
                    None => println!("Failed to find PIX installation"),
                }
            }
        }

        let adapter = create_adapter()?;
        let device = create_device(&adapter)?;

        let compute_commands = create_command_resource(&device, D3D12_COMMAND_LIST_TYPE_COMPUTE)?;

        let resources = D3DResources::new(device.clone())?;

        Ok(D3DGraphics {
            adapter,
            device,
            resources,
            compute_command_list: compute_commands.0,
            compute_command_allocator: compute_commands.1,
            compute_command_queue: compute_commands.2,
            debug,
        })
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
            create_descriptor_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3)?;
        let dsv_descriptor_heap =
            create_descriptor_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1)?;

        let target_views =
            WindowRenderTarget::create_target_views(device, &swap_chain, &rtv_descriptor_heap);
        let depth_stencil_view = WindowRenderTarget::create_depth_stencil_resource(
            device,
            &dsv_descriptor_heap,
            width,
            height,
        );

        Ok(WindowRenderTarget {
            swap_chain,
            command_queue,
            rtv_heap: rtv_descriptor_heap,
            dsv_heap: dsv_descriptor_heap,

            current_buffer: 0,
            frame_signals: [0; NUM_BUFFERS],
            width,
            height,
            target_views,
            depth_stencil_view,
        })
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
        let scissor_rect = RECT {
            left: 0,
            top: 0,
            right: i32::MAX,
            bottom: i32::MAX,
        };
        unsafe {
            command_list.RSSetViewports(&[viewport]);
            command_list.RSSetScissorRects(&[scissor_rect]);
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

        target_views.into_inner().unwrap()
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

        D3DResource {
            resource: depth_buffer,
            cpu_handle,
        }
    }
}

struct Renderable {
    mesh: D3DMesh,
    root_signature: D3DRootSignature,
    pipeline_state: D3DPipelineState,
}

pub struct D3DRenderer<'a> {
    graphics: &'a D3DGraphics,

    draw_command_list: ID3D12GraphicsCommandList,
    draw_command_allocator: ID3D12CommandAllocator,

    render_target: WindowRenderTarget,

    slow_constants: D3DSlowVSConstants,
    fast_constants: D3DFastConstants,

    renderables: Vec<Renderable>,
}

impl<'a> Renderer for D3DRenderer<'a> {
    fn apply(&mut self, items: RenderQueueItems) {
        let mut new_renderables = Vec::new();

        for instance in items.new_meshes {
            let mesh = D3DMesh::load(instance.mesh, &self.graphics.resources).unwrap();
            // let d3d_material = self.graphics.load_material(instance.material);

            let ps = instance.material.shader;
            let root_signature = D3DRootSignature::from_pixel_shader(ps, &self.graphics.device);
            let pipeline_state =
                D3DPipelineState::compile_for_mesh(&self.graphics.device, ps, &root_signature)
                    .unwrap();
            new_renderables.push(Renderable {
                mesh,
                root_signature,
                pipeline_state,
            });
        }
        self.graphics.resources.execute_resource_loads();

        for mut r in new_renderables {
            r.mesh.on_loaded(&self.draw_command_list);
            self.renderables.push(r);
        }
    }

    fn render_frame(&mut self) {
        self.pre_render();
        for renderable in self.renderables.iter() {
            self.draw(renderable);
        }
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

        // setup projection matrix
        const NEAR_CLIP: f32 = 0.5;
        const FAR_CLIP: f32 = 5000.0;

        let aspect_ratio = self.render_target.height as f32 / self.render_target.width as f32;
        self.slow_constants.view =
            Mat4::perspective_lh(1.0, aspect_ratio, NEAR_CLIP, FAR_CLIP).transpose();
        self.slow_constants.world = self.calculate_world_matrix();

        // srv_heap_->Bind(command_list_);
    }

    fn draw(&self, renderable: &Renderable) {
        renderable.root_signature.bind(&self.draw_command_list);

        unsafe {
            let ptr = &self.slow_constants as *const _ as *const c_void;
            self.draw_command_list.SetGraphicsRoot32BitConstants(
                0,
                D3DSlowVSConstants::SIZE_32_BIT,
                ptr,
                0,
            );
        }

        // DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(state.model_.GetMatrix());
        // fast_constants_.model_ = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, state.model_.GetMatrix()));
        unsafe {
            let ptr = &self.fast_constants as *const _ as *const c_void;
            self.draw_command_list.SetGraphicsRoot32BitConstants(
                1,
                D3DFastConstants::SIZE_32_BIT,
                ptr,
                0,
            );
        }

        unsafe {
            self.draw_command_list
                .IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
        };

        // state.material_->Bind(command_list_);
        renderable.pipeline_state.bind(&self.draw_command_list);
        renderable.mesh.draw(&self.draw_command_list);
    }

    fn calculate_world_matrix(&mut self) -> Mat4 {
        let camera_transform = Mat4::IDENTITY;
        camera_transform.inverse().transpose()
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

        Ok(D3DRenderer {
            graphics: self,
            draw_command_list: draw_commands.0,
            draw_command_allocator: draw_commands.1,
            render_target,
            renderables: Vec::new(),
            slow_constants: Default::default(),
            fast_constants: Default::default(),
        })
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
