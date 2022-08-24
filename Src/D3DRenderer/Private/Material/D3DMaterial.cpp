#include "D3DMaterial.h"

D3DMaterial::D3DMaterial(D3DRootSignature* root_signature,
                D3DPipelineState* pipeline_state,
                D3DDescriptorTable* descriptor_table,
                const std::vector<const D3DTexture*>& textures) :
    root_signature_(root_signature), pipeline_state_(pipeline_state), descriptor_table_(descriptor_table), textures_(textures)
{
}

D3DMaterial::~D3DMaterial()
{
    // todo: cleanup root_signature_

    delete pipeline_state_;

    if (descriptor_table_ != nullptr)
    {
        delete descriptor_table_;
    }
}

void D3DMaterial::Bind(ID3D12GraphicsCommandList* command_list)
{
    if (first_bind_)
    {
        first_bind_ = false;

        for (auto t : textures_)
        {
            t->TransitionResource(command_list);
        }
    }

    if (descriptor_table_ != nullptr)
    {
        descriptor_table_->Bind(command_list, 2U);
    }

    pipeline_state_->Bind(command_list);

    // TODO: Bind any textures
}

void D3DMaterial::OnLoadingComplete()
{
    loaded_ = true;
}