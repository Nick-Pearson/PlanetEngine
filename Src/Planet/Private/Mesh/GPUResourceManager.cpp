#include "GPUResourceManager.h"

#include "Mesh.h"
#include "../Renderer/D3DRenderer.h"
#include "../Renderer/D3DShader.h"

GPUResourceManager::GPUResourceManager(Microsoft::WRL::ComPtr <ID3D11Device> device) :
	mDevice(device)
{
}

GPUResourceManager::~GPUResourceManager()
{
}

void GPUResourceManager::LoadMesh(std::shared_ptr<Mesh> mesh)
{
	if (!mesh) return;

	GPUMeshHandle entry;
	entry.mesh = mesh;
	entry.numTriangles = mesh->mTriangles.size();

	// load the buffers
	CreateBuffer((const void*)mesh->mVerticies.data(), mesh->mVerticies.size(), sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER, entry.vertexBuffer);
	CreateBuffer((const void*)mesh->mTriangles.data(), mesh->mTriangles.size(), sizeof(unsigned short), D3D11_BIND_INDEX_BUFFER, entry.triangleBuffer);

	GPUMeshHandle* ptr = mLoadedMeshes.Add(entry);
	mesh->handle = ptr;
}

void GPUResourceManager::UnloadMesh(std::shared_ptr<Mesh> mesh)
{

}

std::shared_ptr<D3DShader> GPUResourceManager::LoadShader(const char* ShaderFile)
{
	std::string key = ShaderFile;

	auto it = loadedShaders.find(key);
	if (it != loadedShaders.end())
	{
		return it->second;
	}

	wchar_t* unicodeStr = new wchar_t[key.size() + 1];
	unicodeStr[key.size()] = 0;
	for (unsigned int i = 0; i < key.size(); ++i)
	{
		unicodeStr[i] = key[i];
	}

	std::shared_ptr<D3DShader> shader = std::make_shared<D3DShader>(unicodeStr, ShaderType::Pixel, mDevice);
	delete unicodeStr;

	loadedShaders.emplace(key, shader);

	return shader;
}

void GPUResourceManager::CreateBuffer(const void* data, size_t length, size_t stride, unsigned int flags, Microsoft::WRL::ComPtr<ID3D11Buffer>& outBuffer)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = flags;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.StructureByteStride = stride;
	bufferDesc.ByteWidth = length * bufferDesc.StructureByteStride;

	D3D11_SUBRESOURCE_DATA resourceData = {};
	resourceData.pSysMem = data;

	d3dAssert(mDevice->CreateBuffer(&bufferDesc, &resourceData, &outBuffer));
}
