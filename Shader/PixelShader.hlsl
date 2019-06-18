cbuffer CBuff_World
{
	float3 sunDir;
	float sunSkyStrength;

	float3 sunCol;
};

float4 main(float3 normal : Color0, float3 worldPos : Color1) : SV_Target
{
	float3 col = float3(1.0f, 1.0f, 1.0f);
	return float4(col * sunCol * max(dot(normal, -sunDir), 0.0f),0.0f);
}