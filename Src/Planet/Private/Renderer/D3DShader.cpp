#include "D3DShader.h"

#pragma comment(lib,"D3DCompiler.lib")

#include <fstream>
#include <iostream>
#include <wrl/client.h>

#include "D3DRenderer.h"

D3DShader::D3DShader(const wchar_t* filename, ShaderType type, ID3D11Device* inDevice) :
	shaderType(type), mDevice(inDevice)
{
	mDevice->AddRef();

	std::wstring fullpath = filename;
	fullpath.insert(0, L"./Shader/");

	const char* shaderTarget = (type == ShaderType::Pixel ? "ps_5_0" : "vs_5_0");
	const unsigned int compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL1;

	Microsoft::WRL::ComPtr<ID3DBlob> ErrorBlob;
	d3dAssert(D3DCompileFromFile(fullpath.c_str(), nullptr, nullptr, "main", shaderTarget, compileFlags, 0u, &mShaderBlob, &ErrorBlob));

	if (ErrorBlob)
	{
		std::cout << "Error compiling shader file: " << filename << std::endl;
		return;
	}

	if (shaderType == ShaderType::Pixel)
	{
		d3dAssert(mDevice->CreatePixelShader(mShaderBlob->GetBufferPointer(), mShaderBlob->GetBufferSize(), nullptr, &mPixelHandle));
	}
	else
	{
		d3dAssert(mDevice->CreateVertexShader(mShaderBlob->GetBufferPointer(), mShaderBlob->GetBufferSize(), nullptr, &mVertexHandle));
	}
}

D3DShader::~D3DShader()
{
	if (mPixelHandle)
		mPixelHandle->Release();

	if (mVertexHandle)
		mVertexHandle->Release();

	if (mDevice)
		mDevice->Release();

	if (mShaderBlob)
		mShaderBlob->Release();
}

void D3DShader::Use(ID3D11DeviceContext* context)
{
	if (shaderType == ShaderType::Pixel)
	{
		context->PSSetShader(mPixelHandle, nullptr, 0u);
	}
	else
	{
		context->VSSetShader(mVertexHandle, nullptr, 0u);
	}
}

