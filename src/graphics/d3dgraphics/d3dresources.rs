use std::alloc::{alloc, dealloc, handle_alloc_error, Layout};
use std::ffi::c_void;
use std::mem::{size_of, ManuallyDrop};
use std::ptr::NonNull;

use windows::core::{Result, Type};
use windows::Win32::Graphics::Direct3D12::*;
use windows::Win32::Graphics::Dxgi::Common::{DXGI_FORMAT_UNKNOWN, DXGI_SAMPLE_DESC};

use super::d3dcommandqueue::{create_command_resource, D3DCommandQueue};

#[derive(Debug, Clone)]
pub struct D3DBuffer {
    pub resource: ID3D12Resource,
    intermediate_resource: ID3D12Resource,
    state: D3D12_RESOURCE_STATES,
}

impl D3DBuffer {
    pub fn get_gpu_virtual_address(&self) -> u64 {
        unsafe { self.resource.GetGPUVirtualAddress() }
    }
    pub fn transition_to(
        &mut self,
        command_list: &ID3D12GraphicsCommandList,
        state: D3D12_RESOURCE_STATES,
    ) {
        let barrier = D3D12_RESOURCE_BARRIER {
            Type: D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            Flags: D3D12_RESOURCE_BARRIER_FLAG_NONE,
            Anonymous: D3D12_RESOURCE_BARRIER_0 {
                Transition: ManuallyDrop::new(D3D12_RESOURCE_TRANSITION_BARRIER {
                    pResource: ManuallyDrop::new(Some(self.resource.clone())),
                    Subresource: D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                    StateBefore: self.state,
                    StateAfter: state,
                }),
            }
            .abi(),
        };

        unsafe { command_list.ResourceBarrier(&[barrier]) }
        self.state = state;
    }
}

#[derive(Default)]
pub struct BufferDesc {
    pub data: Vec<u8>,
    pub element_size: usize,
    pub flags: D3D12_RESOURCE_FLAGS,
    pub heap_type: D3D12_HEAP_TYPE,
}

pub struct D3DResources {
    device: ID3D12Device2,
    command_list: ID3D12GraphicsCommandList,
    command_allocator: ID3D12CommandAllocator,
    command_queue: D3DCommandQueue,
}

impl D3DResources {
    pub fn new(device: ID3D12Device2) -> Result<D3DResources> {
        let copy_commands = create_command_resource(&device, D3D12_COMMAND_LIST_TYPE_COPY)?;
        Ok(D3DResources {
            device,
            command_list: copy_commands.0,
            command_allocator: copy_commands.1,
            command_queue: copy_commands.2,
        })
    }

    fn create_resource(
        &self,
        heap_props: &D3D12_HEAP_PROPERTIES,
        desc: &D3D12_RESOURCE_DESC,
        initial_state: D3D12_RESOURCE_STATES,
    ) -> Result<ID3D12Resource> {
        let mut resource_opt: Option<ID3D12Resource> = None;
        unsafe {
            self.device.CreateCommittedResource(
                heap_props,
                D3D12_HEAP_FLAG_NONE,
                desc,
                initial_state,
                None,
                &mut resource_opt,
            )?;
        }
        Ok(resource_opt.unwrap())
    }

    fn update_subresources(
        &self,
        destination_resource: &ID3D12Resource,
        intermediate_resource: &ID3D12Resource,
        base_offset: u64,
        first_subresource: u32,
        num_subresources: u32,
        src_data: &D3D12_SUBRESOURCE_DATA,
    ) -> Result<u64> {
        const SUBRESOURCE_SIZE: usize =
            size_of::<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>() + size_of::<u32>() + size_of::<u64>();

        let _required_size = 0_u64;
        let mem_to_alloc = SUBRESOURCE_SIZE * num_subresources as usize;

        let layout = Layout::from_size_align(mem_to_alloc, 1).unwrap();
        unsafe {
            let ptr = alloc(layout);
            let playouts = ptr as *mut D3D12_PLACED_SUBRESOURCE_FOOTPRINT;
            if playouts.is_null() {
                handle_alloc_error(layout);
            }
            let prowsizesinbytes = playouts.offset(num_subresources as isize) as *mut u64;
            let pnumrows = prowsizesinbytes.offset(num_subresources as isize) as *mut u32;

            let desc = destination_resource.GetDesc();
            let mut required_size = 0_u64;
            self.device.GetCopyableFootprints(
                &desc,
                first_subresource,
                num_subresources,
                base_offset,
                Some(playouts),
                Some(pnumrows),
                None,
                Some(&mut required_size),
            );

            let result = self.do_update_subresources(
                destination_resource,
                intermediate_resource,
                first_subresource,
                num_subresources,
                required_size,
                playouts,
                pnumrows,
                prowsizesinbytes,
                src_data,
            );

            dealloc(ptr, layout);
            result
        }
    }

    unsafe fn do_update_subresources(
        &self,
        destination_resource: &ID3D12Resource,
        intermediate_resource: &ID3D12Resource,
        first_subresource: u32,
        num_subresources: u32,
        required_size: u64,
        playouts: *mut D3D12_PLACED_SUBRESOURCE_FOOTPRINT,
        pnumrows: *mut u32,
        prowsizesinbytes: *mut u64,
        src_data: &D3D12_SUBRESOURCE_DATA,
    ) -> Result<u64> {
        // Minor validation
        // let intermediate_desc = unsafe { intermediate_resource.GetDesc() };
        let destination_desc = unsafe { destination_resource.GetDesc() };
        // TODO:
        // if (intermediate_desc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
        //     intermediate_desc.Width < RequiredSize + pLayouts[0].Offset ||
        //     RequiredSize > SIZE_T(-1) ||
        //     (destination_desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
        //         (FirstSubresource != 0 || NumSubresources != 1)))
        // {
        //     return 0;
        // }

        let mut pdata: *mut c_void = NonNull::<c_void>::dangling().as_ptr();
        intermediate_resource.Map(0, None, Some(&mut pdata))?;

        for i in 0..num_subresources as isize {
            let row_sizes_in_bytes = prowsizesinbytes.offset(i).read();
            let layouts = playouts.offset(i).read();
            let num_rows = pnumrows.offset(i).read() as usize;

            let dest_data = D3D12_MEMCPY_DEST {
                pData: pdata.offset(layouts.Offset as isize),
                RowPitch: layouts.Footprint.RowPitch as usize,
                SlicePitch: layouts.Footprint.RowPitch as usize * num_rows,
            };

            for z in 0..layouts.Footprint.Depth as isize {
                let p_dest_slice = dest_data.pData.offset(dest_data.SlicePitch as isize * z);
                let p_src_slice = src_data.pData.offset(src_data.SlicePitch * z);
                for y in 0..num_rows as isize {
                    std::ptr::copy_nonoverlapping(
                        p_src_slice.offset(src_data.RowPitch * y),
                        p_dest_slice.offset(dest_data.RowPitch as isize * y),
                        row_sizes_in_bytes as usize,
                    );
                }
            }
        }
        intermediate_resource.Unmap(0, None);

        if destination_desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER {
            let layout = playouts.read();
            self.command_list.CopyBufferRegion(
                destination_resource,
                0,
                intermediate_resource,
                layout.Offset,
                layout.Footprint.Width as u64,
            );
        } else {
            for i in 0..num_subresources {
                let dst = D3D12_TEXTURE_COPY_LOCATION {
                    pResource: ManuallyDrop::new(Some(destination_resource.clone())),
                    Type: D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
                    Anonymous: D3D12_TEXTURE_COPY_LOCATION_0 {
                        SubresourceIndex: i + first_subresource,
                    },
                };
                let src = D3D12_TEXTURE_COPY_LOCATION {
                    pResource: ManuallyDrop::new(Some(intermediate_resource.clone())),
                    Type: D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
                    Anonymous: D3D12_TEXTURE_COPY_LOCATION_0 {
                        PlacedFootprint: playouts.offset(i as isize).read(),
                    },
                };
                unsafe {
                    self.command_list
                        .CopyTextureRegion(&dst, 0, 0, 0, &src, None);
                }
            }
        }
        Ok(required_size)
    }

    pub fn initialise_buffer(&self, buffer: &BufferDesc) -> Result<D3DBuffer> {
        let heap_props = D3D12_HEAP_PROPERTIES {
            Type: buffer.heap_type,
            CPUPageProperty: D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            MemoryPoolPreference: D3D12_MEMORY_POOL_UNKNOWN,
            CreationNodeMask: 1,
            VisibleNodeMask: 1,
        };

        let buffer_size = buffer.data.len() * buffer.element_size;
        let desc = D3D12_RESOURCE_DESC {
            Dimension: D3D12_RESOURCE_DIMENSION_BUFFER,
            Alignment: 0,
            Width: buffer_size as u64,
            Height: 1,
            DepthOrArraySize: 1,
            MipLevels: 1,
            Format: DXGI_FORMAT_UNKNOWN,
            SampleDesc: DXGI_SAMPLE_DESC {
                Count: 1,
                Quality: 0,
            },
            Layout: D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            Flags: buffer.flags,
        };

        let resource = self.create_resource(&heap_props, &desc, D3D12_RESOURCE_STATE_COPY_DEST)?;
        let intermediate_resource =
            self.create_resource(&heap_props, &desc, D3D12_RESOURCE_STATE_GENERIC_READ)?; // flags none, upload heap

        let data = D3D12_SUBRESOURCE_DATA {
            pData: buffer.data.as_ptr() as *const _ as *const c_void,
            RowPitch: buffer_size as isize,
            SlicePitch: buffer.element_size as isize,
        };
        self.update_subresources(&resource, &intermediate_resource, 0, 0, 1, &data)?;

        Ok(D3DBuffer {
            resource,
            intermediate_resource,
            state: D3D12_RESOURCE_STATE_COPY_DEST,
        })
    }
}
