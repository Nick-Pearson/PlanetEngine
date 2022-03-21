#include "D3DMaterial.h"

D3DMaterial::D3DMaterial(const D3DPixelShader* shader, const std::vector<std::shared_ptr<D3DTexture>>& textures, bool alpha) :
    shader_(shader), textures_(textures), alpha_(alpha)
{
}