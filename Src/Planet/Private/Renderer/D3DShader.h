#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>

enum class ShaderType
{
	Vertex,
	Pixel
};

class D3DShader
{
public:
	D3DShader(const wchar_t* filepath, ShaderType type, ID3D11Device* inDevice);
	~D3DShader();

	void Use(ID3D11DeviceContext* context);

	ID3DBlob* mShaderBlob = nullptr;

private:

	void* gpuHandle = nullptr;
	ShaderType shaderType;
	ID3D11Device* mDevice;

	ID3D11PixelShader* mPixelHandle = nullptr;
	ID3D11VertexShader* mVertexHandle = nullptr;
};