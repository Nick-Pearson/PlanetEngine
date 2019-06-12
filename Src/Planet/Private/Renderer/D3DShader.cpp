#include "D3DShader.h"

#pragma comment(lib,"D3DCompiler.lib")

#include <fstream>
#include <iostream>
#include <wrl/client.h>

#include "D3DRenderer.h"
#include "../PlanetLogging.h"

D3DShader::D3DShader(const wchar_t* filename, ShaderType type, Microsoft::WRL::ComPtr <ID3D11Device> inDevice) :
	shaderType(type), mDevice(inDevice)
{
	std::wstring fullpath = filename;
	fullpath.insert(0, L"./Shader/");

	const char* shaderTarget = (type == ShaderType::Pixel ? "ps_5_0" : "vs_5_0");
	const unsigned int compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL1;

	Microsoft::WRL::ComPtr<ID3DBlob> ErrorBlob;
	d3dAssert(D3DCompileFromFile(fullpath.c_str(), nullptr, nullptr, "main", shaderTarget, compileFlags, 0u, &mShaderBlob, &ErrorBlob));

	if (ErrorBlob)
	{
		P_ERROR(Shader, TEXT("Error compiling shader file: ..."), filename);
		//std::cout << (const char*)ErrorBlob->GetBufferPointer() << std::endl;
		return;
	}

	if (shaderType == ShaderType::Pixel)
	{
		d3dAssert(mDevice->CreatePixelShader(mShaderBlob->GetBufferPointer(), mShaderBlob->GetBufferSize(), nullptr, mPixelHandle.GetAddressOf()));
	}
	else
	{
		d3dAssert(mDevice->CreateVertexShader(mShaderBlob->GetBufferPointer(), mShaderBlob->GetBufferSize(), nullptr, mVertexHandle.GetAddressOf()));
	}
}

D3DShader::~D3DShader()
{
	if (mShaderBlob)
		mShaderBlob->Release();
}

void D3DShader::Use(ID3D11DeviceContext* context)
{
	if (shaderType == ShaderType::Pixel)
	{
		context->PSSetShader(mPixelHandle.Get(), nullptr, 0u);
	}
	else
	{
		context->VSSetShader(mVertexHandle.Get(), nullptr, 0u);
	}
}

