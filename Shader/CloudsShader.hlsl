cbuffer CBuff_World
{
	float3 sunDir;
	float sunSkyStrength;

	float3 sunCol;
};

float4 main(float3 normal : Color0, float3 worldPos : Color1, float2 texCoord : TexCoord) : SV_Target
{
	return float4(1.0f, 0.0f, 1.0f, 0.1f);
}