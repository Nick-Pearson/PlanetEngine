#include "D3DTexture.h"

#include "../../Texture/Texture2D.h"

#include <d3d11.h>

D3DTexture::D3DTexture(const Texture2D* texture, Microsoft::WRL::ComPtr <ID3D11Device> inDevice)
{
	// create texture resource
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = texture->GetWidth();
	textureDesc.Height = texture->GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = texture->GetData();
	sd.SysMemPitch = texture->GetWidth() * sizeof(Colour);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> d3d11Texture;
	inDevice->CreateTexture2D(&textureDesc, &sd, &d3d11Texture);

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	inDevice->CreateShaderResourceView(d3d11Texture.Get(), &srvDesc, &mTextureView);
}

D3DTexture::~D3DTexture()
{
}

void D3DTexture::Use(ID3D11DeviceContext* context, int slot)
{
	context->PSSetShaderResources(slot, 1u, mTextureView.GetAddressOf());
}