// atmosphere radius (in km)
#define RA 6420e3f
// earth radius (in km)
#define RE 6360e3f

//Thickness of the atmosphere if density was uniform for Rayliegh and Mie
#define HR 7994.0f
#define HM 1200.0f

#define NUM_SAMPLES 16
#define NUM_SAMPLES_LIGHT 8

#define PI 3.14159265359f

#define BETA_R float3(5.5e-6f, 13.0e-6f, 22.4e-6f)
#define BETA_M float3(21e-6f, 21e-6f, 21e-6f)

cbuffer CBuff_World
{
	float3 sunDir;
	float sunSkyStrength;

	float3 sunCol;
};

float intersect_atmosphere(float3 orig, float3 dir)
{
	float3 rc = -orig;

	float tca = dot(rc, dir);
	float d2 = dot(rc, rc) - (tca * tca);

	const float radius2 = RA * RA;
	
	// shouldn't happen
	if (d2 > radius2) return -1.0f;

	return tca + sqrt(radius2 - d2);
}

float rayleigh_phase(float mu)
{
	return (3.f + (3.f * mu * mu)) / (16.f * PI);
}

float mie_phase(float mu)
{
	const float g = 0.76f;
	const float g2 = g * g;
	return (1.0f - g2) / ((4.0f+PI) * pow(abs(1.0f + g2 - (2.0f*g*mu)), 1.5f));
}

float4 main(float3 normal : Color0, float3 worldPos : Color1) : SV_Target
{
	//float3 sunDir = normalize(float3(0.0f, -.03f, 1.0f));

	float3 dir = -normal;
	float3 orig = float3(0.0f, RE + 1.0f, 0.0f);

	// work out the amount of atmosphere we are looking through
	float t = intersect_atmosphere(orig, dir);

	const float segmentLen = t / float(NUM_SAMPLES);
	const float mu = dot(dir, sunDir);
	
	const float phaseR = rayleigh_phase(mu);
	const float phaseM = mie_phase(mu);
	
	float opticalDepthR = 0.0f;
	float opticalDepthM = 0.0f;
	float tCurrent = 0.0f;

	float3 sumR = float3(0.0f, 0.0f, 0.0f);
	float3 sumM = float3(0.0f, 0.0f, 0.0f);

	[loop]
	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		float3 samplePosition = orig + (tCurrent + segmentLen * 0.5f) * dir;
		float height = length(samplePosition) - RE;

		// compute optical depth for light
		float hr = exp(-height / HR) * segmentLen;
		float hm = exp(-height / HM) * segmentLen;
		opticalDepthR += hr;
		opticalDepthM += hm;

		// calculate sun light
		float tLight = intersect_atmosphere(samplePosition, sunDir);
		if (tLight < 0.0f) return float4(1.0f, 0.0f, 1.0f, 1.0f);
		float segmentLengthLight = tLight / float(NUM_SAMPLES_LIGHT);
		float tCurrentLight = 0.0f;
		float opticalDepthLightR = 0.0f;
		float opticalDepthLightM = 0.0f;

		int j = 0;

		[unroll(NUM_SAMPLES_LIGHT)]
		for (; j < NUM_SAMPLES_LIGHT; ++j)
		{
			float3 samplePositionLight = samplePosition + (tCurrentLight + segmentLengthLight * 0.5f) * sunDir;
			float heightLight = length(samplePositionLight) - RE;

			if (heightLight < 0.0f) break;

			opticalDepthLightR += exp(-heightLight / HM) * segmentLengthLight;
			opticalDepthLightM += exp(-heightLight / HM) * segmentLengthLight;

			tCurrentLight += segmentLengthLight;
		}

		if (j == NUM_SAMPLES_LIGHT)
		{
			float3 tau = BETA_R * (opticalDepthR + opticalDepthLightR) +
				BETA_M * 1.1f * (opticalDepthM + opticalDepthLightM);
			float3 attenuation = exp(-tau);

			sumR += hr * attenuation;
			sumM += hm * attenuation;
		}

		tCurrent += segmentLen;
	}

	float3 col = (sumR * phaseR * BETA_R + sumM * phaseM * BETA_M) * sunSkyStrength;
	return float4(col, 1.0f);
}