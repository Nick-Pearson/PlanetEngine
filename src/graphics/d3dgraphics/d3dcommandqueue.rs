use std::cell::Cell;

use windows::{
    core::*, Win32::Foundation::*, Win32::Graphics::Direct3D12::*, Win32::System::Threading::*,
};

pub struct D3DCommandQueue {
    next_signal: Cell<u64>,
    last_completed: Cell<u64>,
    fence: ID3D12Fence,
    pub command_queue: ID3D12CommandQueue,
    event: HANDLE,
}

impl D3DCommandQueue {
    pub fn new(
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
