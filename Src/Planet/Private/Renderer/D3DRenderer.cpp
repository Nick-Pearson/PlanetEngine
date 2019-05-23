#include "D3DRenderer.h"

#include "../Platform/Window.h"
#include "D3DShader.h"
#include <wrl/client.h>
#include <D3Dcompiler.h>
#include <xutility>

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

	// TODO: Bind to event on window changed size

	// Compile Shaders
	vertexShader = new D3DShader{ L"VertexShader.hlsl", ShaderType::Vertex, mDevice };
	pixelShader = new D3DShader{ L"PixelShader.hlsl", ShaderType::Pixel, mDevice };

	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	// get address of DXGIGetDebugInterface in dll
	typedef HRESULT (WINAPI* DXGIGetDebugInterface)(REFIID, IDXGIInfoQueue **);
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));

	DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &mDxgiInfoQueue);
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
}

void D3DRenderer::SwapBuffers()
{
	mSwapChain->Present(1u, 0u);

	const float colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mContext->ClearRenderTargetView(mTarget, colour);
}

void D3DRenderer::Render()
{
	struct Vertex
	{
		Vertex(float inX, float inY) :
			x(inX), y(inY)
		{}

		float x;
		float y;
	};

	// create vertex buffer (1 2d triangle at center of screen)
	const Vertex vertices[] =
	{
		{ 0.0f,0.5f },
		{ 0.5f,-0.5f },
		{ -0.5f,-0.5f },
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	d3dAssert(mDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	
	const unsigned int stride = sizeof(Vertex);
	const unsigned int offset = 0u;
	mContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	pixelShader->Use(mContext);
	vertexShader->Use(mContext);

	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
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
	mContext->Draw((UINT)std::size(vertices), 0u);
	d3dFlushDebugMessages();
}
