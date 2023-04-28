struct VS_Out
{
	float4 position : SV_Position;
	float3 normal : Color0;
	float2 texCoord : TexCoord;
};

struct VS_In
{
    float3 position : Position;
    float3 normal : Normal;
    float2 texCoord : TexCoord;
};

struct SlowConstants
{
    matrix world;
    matrix view;
};
ConstantBuffer<SlowConstants> slow : register(b0);

struct FastConstants
{
    matrix model;
};
ConstantBuffer<FastConstants> fast : register(b1);

VS_Out main(VS_In input)
{
	VS_Out output;

	matrix modelWorldView = mul(mul(fast.model, slow.world), slow.view);
	
	output.position = mul(float4(input.position, 1.0f), modelWorldView);
	output.normal = normalize(mul(input.normal, (float3x3)fast.model));
	output.texCoord = float2(input.texCoord.x, input.texCoord.y);

	return output;
}