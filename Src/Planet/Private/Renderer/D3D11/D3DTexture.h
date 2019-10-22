#pragma once

#include <wrl.h>

class Texture2D;

class D3DTexture
{
public:
	D3DTexture(const Texture2D* texture, Microsoft::WRL::ComPtr <struct ID3D11Device> inDevice);
	~D3DTexture();

	void Use(struct ID3D11DeviceContext* context, int slot);

private:

	Microsoft::WRL::ComPtr <struct ID3D11SamplerState> mSamplerState;
	Microsoft::WRL::ComPtr <struct ID3D11ShaderResourceView> mTextureView;
};