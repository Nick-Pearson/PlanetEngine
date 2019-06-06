#pragma once

#include "../Platform/PlanetWindows.h"
#include <d3d11.h>
#include <wrl.h>

#include "../Container/LinkedList.h"

#include <memory>

class Mesh;

struct GPUMeshHandle
{
	std::shared_ptr<Mesh> mesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> triangleBuffer;
	unsigned int numTriangles;

	bool operator==(const GPUMeshHandle& other) const
	{
		return other.mesh == mesh;
	}
};

class MeshManager
{
public:

	MeshManager(Microsoft::WRL::ComPtr <ID3D11Device> device);
	~MeshManager();

	void LoadMesh(std::shared_ptr<Mesh> mesh);
	void UnloadMesh(std::shared_ptr<Mesh> mesh);

private:

	void CreateBuffer(const void* data, size_t length, size_t stride, unsigned int flags, Microsoft::WRL::ComPtr<ID3D11Buffer>& outBuffer);

	LinkedList <GPUMeshHandle> mLoadedMeshes;

	Microsoft::WRL::ComPtr <ID3D11Device> mDevice;
};