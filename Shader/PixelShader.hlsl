float4 main(float3 normal : Color0, float3 worldPos : Color1) : SV_Target
{
	float3 lightPos = float3(0.0f , 0.0f, -5.0f);
	float3 lightDir = worldPos - lightPos;
	float dist = dot(lightDir, lightDir);
	lightDir = normalize(lightDir);
	float light = 4.0f * 3.1415f / dist;

	float3 col = float3(1.0f, 1.0f, 1.0f);
	return float4(col * light * max(dot(normal, lightDir), 0.0f),0.0f);
	//return float4(normal,0.0f);
}