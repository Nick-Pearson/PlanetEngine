#include "D3DDescriptorTable.h"

D3DDescriptorTable::D3DDescriptorTable(const DescriptorHandle& handle, size_t size) :
    handle_(handle), size_(size)
{
}

D3DDescriptorTable::~D3DDescriptorTable()
{
}

void D3DDescriptorTable::Bind(ID3D12GraphicsCommandList* command_list, uint32_t slot)
{
    command_list->SetGraphicsRootDescriptorTable(slot, handle_.gpu_);
}
