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
	float2 texCoord : TexCoord;
	float4 position : SV_Position;
};

VS_Out main(float3 pos : Position, float3 norm : Normal, float2 texCoord : TexCoord)
{
	VS_Out res;

	matrix modelWorldView = mul(mul(model, world), view);
	
	res.position = mul(float4(pos, 1.0f), modelWorldView);

	float4 translatedPos = mul(float4(pos, 1.0f), model);
	res.normal = normalize(mul(norm, (float3x3)model));
	res.worldPos = float3(translatedPos.x, translatedPos.y, translatedPos.z);
	res.texCoord = float2(texCoord.x, texCoord.y);

	return res;
}