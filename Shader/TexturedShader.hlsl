// cbuffer CBuff_World
// {
// 	float3 sunDir;
// 	float sunSkyStrength;

// 	float3 sunCol;
// };

Texture2D tex : register(t0);
SamplerState splr : register(s0);

float4 main(float4 position : SV_Position, float3 normal : Color0, float2 texCoord : TexCoord) : SV_Target
{
	float4 col = tex.Sample(splr, float2(texCoord.x, 1.0f - texCoord.y));
	// float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// return float4(col.xyz * sunCol * max(dot(normal, -sunDir), 0.0f), 1.0f);
	return float4(col.xyz, 1.0f);
}