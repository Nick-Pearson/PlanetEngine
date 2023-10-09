struct WorldConstants
{
    float4x4 world;
    float4x4 view;
	float3 sunDir;
	float sunSkyStrength;
	float3 sunCol;
};

struct MeshInstanceConstants
{
    float4x4 model;
};

ConstantBuffer<WorldConstants> world : register(b0);
ConstantBuffer<MeshInstanceConstants> instance : register(b1);