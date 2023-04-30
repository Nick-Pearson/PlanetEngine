struct FastConstants
{
    matrix model;
	float3 sunDir;
	float sunSkyStrength;
	float3 sunCol;
};
ConstantBuffer<FastConstants> fast : register(b1);