#include "D3DTexture.h"

D3DTexture::D3DTexture(ID3D12Resource* resource, ID3D12Resource* intermediate_resource, DXGI_FORMAT format) :
    resource_(resource), intermediate_resource_(intermediate_resource), format_(format)
{
}

D3DTexture::~D3DTexture()
{
}

void D3DTexture::OnLoadingComplete()
{
    loaded_ = true;
}