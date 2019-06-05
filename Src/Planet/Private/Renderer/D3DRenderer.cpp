#include "D3DRenderer.h"

#include "../Platform/Window.h"
#include "D3DShader.h"
#include <wrl/client.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "../Mesh/MeshManager.h"
#include "../Mesh/Mesh.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "dxguid.lib")

D3DRenderer::D3DRenderer(const Window& targetWindow)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = targetWindow.GetWindowHandle();
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// create device and front/back buffers, and swap chain and rendering context
	unsigned int createFlags = D3D11_CREATE_DEVICE_DEBUG;

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&mSwapChain,
		&mDevice,
		nullptr,
		&mContext
	);

	// setup viewport and buffers
	ID3D11Resource* BackBuffer = nullptr;
	d3dAssert(mSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&BackBuffer));
	d3dAssert(mDevice->CreateRenderTargetView(BackBuffer, nullptr, &mTarget));
	BackBuffer->Release();

	mContext->OMSetRenderTargets(1u, &mTarget, nullptr);

	D3D11_VIEWPORT vp;
	vp.Width = (float)targetWindow.GetSizeX();
	vp.Height = (float)targetWindow.GetSizeY();
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mContext->RSSetViewports(1u, &vp);

	aspectRatio = vp.Height / vp.Width;

	// TODO: Bind to event on window changed size

	// Compile Shaders
	vertexShader = new D3DShader{ L"VertexShader.hlsl", ShaderType::Vertex, mDevice };
	pixelShader = new D3DShader{ L"PixelShader.hlsl", ShaderType::Pixel, mDevice };

	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	d3dAssert(mDevice->CreateInputLayout(
		ied, (UINT)std::size(ied),
		vertexShader->mShaderBlob->GetBufferPointer(),
		vertexShader->mShaderBlob->GetBufferSize(),
		&InputLayout
	));

	// bind vertex layout
	mContext->IASetInputLayout(InputLayout.Get());
	mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	// get address of DXGIGetDebugInterface in dll
	typedef HRESULT (WINAPI* DXGIGetDebugInterface)(REFIID, IDXGIInfoQueue **);
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));

	DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &mDxgiInfoQueue);

	mMeshManager = new MeshManager{ mDevice };
}

D3DRenderer::~D3DRenderer()
{
	if (mContext)
		mContext->Release();
	
	if (mSwapChain)
		mSwapChain->Release();
	
	if (mDevice)
		mDevice->Release();

	delete vertexShader;
	delete pixelShader;
	delete mMeshManager;
}

void D3DRenderer::SwapBuffers()
{
	mSwapChain->Present(1u, 0u);

	const float colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mContext->ClearRenderTargetView(mTarget, colour);
}

void D3DRenderer::RenderMesh(std::shared_ptr<Mesh> mesh)
{
	if (!mesh) return;

	GPUMeshHandle* meshHandle = mesh->GetGPUHandle();
	if (!meshHandle) return;

	struct ConstantBuffer
	{
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
	};
	const ConstantBuffer cb =
	{
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixRotationX(angle) *
			DirectX::XMMatrixRotationY(-angle * 0.5f)
		),
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f) *
			DirectX::XMMatrixTranslation(3.0f,0.0f,6.0f)
		),
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixPerspectiveLH(1.0f,aspectRatio,0.5f,5000.0f)
		)
	};
	angle += 0.02f;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	d3dAssert(mDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	mContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	const unsigned int stride = sizeof(Vertex);
	const unsigned int offset = 0u;
	mContext->IASetVertexBuffers(0u, 1u, meshHandle->vertexBuffer.GetAddressOf(), &stride, &offset);
	mContext->IASetIndexBuffer(meshHandle->triangleBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	pixelShader->Use(mContext);
	vertexShader->Use(mContext);

	mContext->DrawIndexed(meshHandle->numTriangles, 0u, 0u);
	d3dFlushDebugMessages();
}
