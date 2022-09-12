struct WorldConstants
{
	float3 sunDir;
	float sunSkyStrength;

	float3 sunCol;
};

Texture2D albedoMap : register(t0);
Texture2D normalMap : register(t1);
SamplerState splr : register(s0);
ConstantBuffer<WorldConstants> world : register(b0);

float4 main(float4 position : SV_Position, float3 normal : Color0, float2 texCoord : TexCoord) : SV_Target
{
	float4 col = albedoMap.Sample(splr, float2(texCoord.x, 1.0f - texCoord.y));
	return float4(col.xyz * world.sunCol * max(dot(normal, -world.sunDir), 0.0f), 1.0f);
}