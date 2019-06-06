#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>
#include <wrl/client.h>

enum class ShaderType
{
	Vertex,
	Pixel
};

class D3DShader
{
public:
	D3DShader(const wchar_t* filepath, ShaderType type, Microsoft::WRL::ComPtr <ID3D11Device> inDevice);
	~D3DShader();

	void Use(ID3D11DeviceContext* context);

	ID3DBlob* mShaderBlob = nullptr;

private:

	void* gpuHandle = nullptr;
	ShaderType shaderType;
	Microsoft::WRL::ComPtr <ID3D11Device> mDevice;

	Microsoft::WRL::ComPtr <ID3D11PixelShader> mPixelHandle;
	Microsoft::WRL::ComPtr <ID3D11VertexShader> mVertexHandle;
};