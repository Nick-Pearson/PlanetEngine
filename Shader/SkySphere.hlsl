float4 main(float3 normal : Color0, float3 worldPos : Color1) : SV_Target
{
	float3 col = float3(0.0f, 0.1f, 0.8f);
	return float4(col,0.0f);
}