cbuffer CBuff_World
{
	float3 sunDir;
	float sunSkyStrength;

	float3 sunCol;
};


Texture2D tex;

SamplerState splr;

float4 main(float3 normal : Color0, float3 worldPos : Color1, float2 texCoord : TexCoord) : SV_Target
{
	float4 col = tex.Sample(splr, texCoord);
	return float4(col.xyz * sunCol * max(dot(normal, -sunDir), 0.0f), 1.0f);
}