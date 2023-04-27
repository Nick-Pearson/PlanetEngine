struct WorldConstants
{
	float3 sunDir;
	float sunSkyStrength;

	float3 sunCol;
};

ConstantBuffer<WorldConstants> world : register(b0);

float4 main(float4 position : SV_Position, float3 normal : Color0, float2 texCoord : TexCoord) : SV_Target
{
	return float4(world.sunCol, 1.0f);
}