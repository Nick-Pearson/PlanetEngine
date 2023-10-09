#include "VSCommon.hlsl"

VS_Out main(VS_In input)
{
	VS_Out output;

	matrix modelWorldView = mul(mul(instance.model, world.world), world.view);
	
	output.position = mul(float4(input.position, 1.0f), modelWorldView);
	output.normal = normalize(mul(input.normal, (float3x3)instance.model));
	output.texCoord = float2(input.texCoord.x, input.texCoord.y);

	return output;
}