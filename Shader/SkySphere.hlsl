// atmosphere radius (in km)
#define RA 6420000.0f
// earth radius (in km)
#define RE 6360000.0f

//Thickness of the atmosphere if density was uniform for Rayliegh and Mie
#define HR 7994.0f
#define HM 1200.0f

#define NUM_SAMPLES 16
#define NUM_SAMPLES_LIGHT 8

#define PI 3.14159265359f

#define BETA_R float3(3.8e-6f, 13.5e-6f, 33.1e-6f)
#define BETA_M float3(21e-6f, 21e-6f, 21e-6f)

cbuffer CBuff_World
{
	float3 sunDir;
	float sunSkyStrength;

	float3 sunCol;
};

float intersect_atmosphere(float3 orig, float3 dir, out float t0, out float t1)
{
	const float3 rc = -orig; // -100
	const float radius2 = RA * RA; // 400000

	float tca = dot(rc, dir);
	float d2 = dot(rc, rc) - (tca * tca);
	
	// shouldn't happen
	if (d2 > radius2) return false;

	float thc = sqrt(radius2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;
	return true;
}

float rayleigh_phase(float mu)
{
	return 3.f / (16.f * PI) * (1 + mu * mu);
}

float mie_phase(float mu)
{
	const float g = 0.76f;
	const float g2 = g * g;
	return 3.f / (8.f * PI) * ((1.f - g2) * (1.f + mu * mu)) / ((2.f + g2) * pow(abs(1.0f + g2 - 2.0f * g * mu), 1.3f));
}

float4 main(float3 normal : Color0) : SV_Target
{
	float3 dir = normalize(-normal);
	float3 orig = float3(0.0f, RE * 1.0f, 0.0f);

	// work out the amount of atmosphere we are looking through
	float t0, t1;
	if (!intersect_atmosphere(orig, dir, t0, t1))
	{
		return float4(1.0f, 0.0f, 1.0f, 1.0f);
	}
	t0 = max(t0, 0);

	const float segmentLen = (t1 - t0) / float(NUM_SAMPLES);
	const float mu = dot(dir, sunDir);
	
	const float phaseR = rayleigh_phase(mu);
	const float phaseM = mie_phase(mu);
	
	float opticalDepthR = 0.0f;
	float opticalDepthM = 0.0f;
	float tCurrent = t0;

	float3 sumR = float3(0.0f, 0.0f, 0.0f);
	float3 sumM = float3(0.0f, 0.0f, 0.0f);

	[loop]
	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		float3 samplePosition = orig + (tCurrent + segmentLen * 0.5f) * dir;
		float height = length(samplePosition) - RE;

		// compute optical depth for light
		const float hr = exp(-height / HR) * segmentLen;
		const float hm = exp(-height / HM) * segmentLen;
		opticalDepthR += hr;
		opticalDepthM += hm;

		// calculate sun light
		float t0Light, t1Light;
		if (!intersect_atmosphere(samplePosition, sunDir, t0Light, t1Light))
		{
			return float4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		float segmentLengthLight = t1Light / float(NUM_SAMPLES_LIGHT);
		float tCurrentLight = 0.0f;
		float opticalDepthLightR = 0.0f;
		float opticalDepthLightM = 0.0f;

		int j = 0;

		[loop]
		for (; j < NUM_SAMPLES_LIGHT; ++j)
		{
			float3 samplePositionLight = samplePosition + (tCurrentLight + segmentLengthLight * 0.5f) * sunDir;
			float heightLight = length(samplePositionLight) - RE;

			if (heightLight < 0.0f) break;

			opticalDepthLightR += exp(-heightLight / HR) * segmentLengthLight;
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

	float3 col = float3(0.0f, 0.0f, 0.0f);
	col += sumR * phaseR * BETA_R;
	col += sumM * phaseM * BETA_M;
	return float4(col * sunSkyStrength, 1.0f);
}