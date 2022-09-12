struct WorldConstants
{
	float3 sunDir;
	float sunSkyStrength;

	float3 sunCol;
};

ConstantBuffer<WorldConstants> world : register(b0);

float4 main(float4 position : SV_Position, float3 normal : Color0, float2 texCoord : TexCoord) : SV_Target
{
	float3 col = float3(1.0f, 1.0f, 1.0f);
	return float4(col * world.sunCol * max(dot(normal, -world.sunDir), 0.0f), 1.0f);
}