#include "GPUResourceManager.h"

#include "Mesh.h"
#include "../Renderer/D3D11/D3DRenderer.h"
#include "../Renderer/D3D11/D3DShader.h"
#include "../Renderer/D3D11/D3DTexture.h"
#include "../Material/Material.h"
#include "../Texture/Texture2D.h"

GPUResourceManager::GPUResourceManager(Microsoft::WRL::ComPtr <ID3D11Device> device) :
	mDevice(device)
{
}

GPUResourceManager::~GPUResourceManager()
{
}

GPUMeshHandle* GPUResourceManager::LoadMesh(std::shared_ptr<Mesh> mesh)
{
	if (!mesh) return nullptr;

	GPUMeshHandle entry;
	entry.mesh = mesh;
	entry.numTriangles = mesh->mTriangles.size();

	// load the buffers
	CreateBuffer((const void*)mesh->mVerticies.data(), mesh->mVerticies.size(), sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER, entry.vertexBuffer);
	CreateBuffer((const void*)mesh->mTriangles.data(), mesh->mTriangles.size(), sizeof(unsigned short), D3D11_BIND_INDEX_BUFFER, entry.triangleBuffer);

	return mLoadedMeshes.Add(entry);
}

void GPUResourceManager::UnloadMesh(std::shared_ptr<Mesh> mesh)
{

}

std::shared_ptr<GPUMaterialHandle> GPUResourceManager::LoadMaterial(std::shared_ptr<Material> material)
{
	auto existing = mLoadedMaterials.find(material->GetShaderPath());
	if (existing != mLoadedMaterials.end())
	{
		P_LOG("Reusing existing loaded shader for %s", material->GetShaderPath().c_str());
		return existing->second;
	}

	std::shared_ptr <D3DShader> compiledShader = LoadShader(material->GetShaderPath(), false);

	std::shared_ptr<GPUMaterialHandle> entry = std::make_shared<GPUMaterialHandle>();
	entry->material = material;
	entry->shader = compiledShader;

	int numTextures = material->GetNumTextures();
	for (int i = 0; i < numTextures; ++i)
	{
		std::shared_ptr<Texture2D> texture = material->GetTextureAt(i);
		entry->textures.push_back(std::make_shared<D3DTexture>(texture.get(), mDevice));
	}

	mLoadedMaterials.emplace(material->GetShaderPath(), entry);
	return entry;
}

void GPUResourceManager::ReloadAllShaders()
{
	P_LOG("Reloading all shaders");
	for (auto i : mLoadedMaterials)
	{
		std::shared_ptr <D3DShader> compiledShader = LoadShader(i.first, true);
		i.second->shader = compiledShader;
	}
}

std::shared_ptr<D3DShader> GPUResourceManager::LoadShader(const std::string& shaderFile, bool force)
{
	if (!force)
	{
		auto it = loadedShaders.find(shaderFile);
		if (it != loadedShaders.end())
		{
			return it->second;
		}
	}

	P_LOG("Loading shader file %s", shaderFile.c_str());

	wchar_t* unicodeStr = new wchar_t[shaderFile.size() + 1];
	unicodeStr[shaderFile.size()] = 0;
	for (unsigned int i = 0; i < shaderFile.size(); ++i)
	{
		unicodeStr[i] = shaderFile[i];
	}

	std::shared_ptr<D3DShader> shader = std::make_shared<D3DShader>(unicodeStr, ShaderType::Pixel, mDevice);
	delete unicodeStr;

	loadedShaders.emplace(shaderFile, shader);

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
