cbuffer CBuff_World
{
	float3 sunDir;
	float sunSkyStrength;

	float3 sunCol;
};

Texture3D low_freq_tex : register(t0);
SamplerState low_freq_splr : register(s0);
Texture3D high_freq_tex : register(t1);
SamplerState high_freq_splr : register(s1);

// maps a value from one range to another
float remap(float original_value, float original_min, float original_max, float new_min, float new_max)
{
	const float original_range = original_max - original_min;
	const float new_range = new_max - new_min;
	return new_min + (((original_value - original_min) / original_range) * new_range);
}

float4 main(float3 normal : Color0, float3 worldPos : Color1, float2 texCoord : TexCoord) : SV_Target
{
	float4 low_freq = low_freq_tex.Sample(low_freq_splr, float3(texCoord, 0.0f));
	float4 high_freq = high_freq_tex.Sample(high_freq_splr, float3(texCoord, 0.0f));
	float val = max(0.0f, high_freq.w);
	return float4(val, val, val, 1.0f);
}