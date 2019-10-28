#pragma once

#include "Platform/PlanetWindows.h"
#include <d3d11.h>
#include <wrl.h>

#include "../Container/LinkedList.h"

#include <memory>
#include <unordered_map>

class Mesh;
class Material;
class D3DShader;
class D3DTexture;

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

struct GPUMaterialHandle
{
	std::shared_ptr<Material> material;
	std::shared_ptr<D3DShader> shader;
	std::vector<std::shared_ptr<D3DTexture>> textures;
};

class GPUResourceManager
{
public:

	GPUResourceManager(Microsoft::WRL::ComPtr <ID3D11Device> device);
	~GPUResourceManager();

	void LoadMesh(std::shared_ptr<Mesh> mesh);
	void UnloadMesh(std::shared_ptr<Mesh> mesh);

	void LoadMaterial(std::shared_ptr<Material> material);

private:
	
	std::shared_ptr<D3DShader> LoadShader(const char* ShaderFile);

	void CreateBuffer(const void* data, size_t length, size_t stride, unsigned int flags, Microsoft::WRL::ComPtr<ID3D11Buffer>& outBuffer);

	LinkedList <GPUMeshHandle> mLoadedMeshes;
	LinkedList <GPUMaterialHandle> mLoadedMaterials;

	std::unordered_map<std::string, std::shared_ptr<D3DShader>> loadedShaders;

	Microsoft::WRL::ComPtr <ID3D11Device> mDevice;
};