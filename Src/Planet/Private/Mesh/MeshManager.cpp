#include "MeshManager.h"

#include "Mesh.h"
#include "../Renderer/D3DRenderer.h"

MeshManager::MeshManager(Microsoft::WRL::ComPtr <ID3D11Device> device) :
	mDevice(device)
{
}

MeshManager::~MeshManager()
{
}

void MeshManager::LoadMesh(std::shared_ptr<Mesh> mesh)
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

void MeshManager::UnloadMesh(std::shared_ptr<Mesh> mesh)
{

}

void MeshManager::CreateBuffer(const void* data, size_t length, size_t stride, unsigned int flags, Microsoft::WRL::ComPtr<ID3D11Buffer>& outBuffer)
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
