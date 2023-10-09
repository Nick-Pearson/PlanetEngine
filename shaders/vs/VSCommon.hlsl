#include "../Common.hlsl"

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