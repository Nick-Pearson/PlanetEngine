cbuffer CBufSlow
{
	matrix world;
	matrix view;
};

cbuffer CBufFast
{
	matrix model;
};

struct VS_Out
{
	float3 normal : Color0;
	float3 worldPos : Color1;
	float4 position : SV_Position;
};

VS_Out main(float3 pos : Position, float3 norm : Normal)
{
	VS_Out res;

	matrix modelWorldView = mul(world, view);
	
	res.position = mul(float4(pos, 1.0f), modelWorldView);

	float4 translatedPos = mul(float4(pos, 1.0f), model);
	res.normal = normalize(mul(norm, (float3x3)model));
	res.worldPos = float3(translatedPos.x, translatedPos.y, translatedPos.z);

	return res;
}