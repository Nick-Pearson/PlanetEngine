// atmosphere radius (in m)
#define RA 6420000.0f
#define RE 6360000.0f
// earth radius (in km)
#define RE_KM 3360.0f

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

Texture3D low_freq_tex : register(t0);
SamplerState low_freq_splr : register(s0);
Texture3D high_freq_tex : register(t1);
SamplerState high_freq_splr : register(s1);

static const float CloudScale = 0.005f;
static const float2 CloudOffset = float2(0.0f, 0.0f);
static const float CloudCoverage = 0.7f;
static const float DensityThreshold = 0.5f;
static const float DensityMultiplier = 10.0f;
static const float LightAbsorptionTowardSun = 0.84f;
static const float G = 0.1f;
static const float CloudHeightKM = 15.0f;
static const float CloudDepthKM = 35.0f;
static const float StepSize = (CloudDepthKM / 8);

float intersect_atmosphere(float3 orig, float3 dir, out float t0, out float t1)
{
	const float3 rc = -orig;
	const float radius2 = RA * RA;

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

float3 sky_col(float3 dir)
{
	float3 orig = float3(0.0f, RE, 0.0f);

	// work out the amount of atmosphere we are looking through
	float t0, t1;
	if (!intersect_atmosphere(orig, dir, t0, t1))
	{
		return float3(1.0f, 0.0f, 1.0f);
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
			return float3(0.0f, 1.0f, 0.0f);
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
	return col * sunSkyStrength;
}

/*
Finds depth to the edge of the cloud sphere

Min distance of 
    - Distance to the edge of the outer sphere
    - Distance to the start of the inner sphere (could be 0)
*/
float get_cloud_depth(float3 orig, float3 dir)
{
    const float inner_shell_raidus = RE_KM + CloudHeightKM;
    const float outer_shell_raidus = inner_shell_raidus + CloudDepthKM;

	const float3 rc = -orig;
	const float inner_radius2 = inner_shell_raidus * inner_shell_raidus;
	const float outer_radius2 = outer_shell_raidus * outer_shell_raidus;

	float tca = dot(rc, dir);
	float d2 = dot(rc, rc) - (tca * tca);
	
	// shouldn't happen
	if (d2 > outer_radius2) return 0.0f;

	float outer_thc = sqrt(outer_radius2 - d2);
    float outer_dist =  max(0.0f, tca + outer_thc);
	
	if (d2 < inner_radius2)
    {
	    float inner_thc = sqrt(inner_radius2 - d2);
        float inner_dist = tca - inner_thc;

        if (inner_dist > 0.0f)
        {
            return min(inner_dist, outer_dist);
        }
    }
        
    return outer_dist;
}

float remap(float orig, float orig_min, float orig_max, float new_min, float new_max)
{
	float ratio = (orig - orig_min) / (orig_max - orig_min);
	return new_min + (ratio * (new_max - new_min));
}

float get_height_fraction(float3 position)
{
	return saturate((position.y - CloudHeightKM) / CloudDepthKM);
}

float sample_cloud_density(float3 position)
{
    float3 uvw;
	uvw.x = (position.x * CloudScale) + (CloudOffset.x * 0.01f);
	uvw.y = (position.y - CloudHeightKM) / CloudDepthKM;
	uvw.z = (position.z * CloudScale) + (CloudOffset.y * 0.01f);

    float4 low_freq = low_freq_tex.SampleLevel(low_freq_splr, uvw, 0);

	float low_freq_FBM = (low_freq.g * 0.625f) +
						(low_freq.b * 0.250f) +
						(low_freq.a * 0.125f);

	float base_cloud = remap(low_freq.r, low_freq_FBM - 1.0f, 1.0, 0.0f, 1.0f);

    // float4 high_freq = high_freq_tex.Sample(high_freq_splr, userToCloud);
    // float val = low_freq.x;

	float base_cloud_with_coverage = remap(base_cloud, CloudCoverage, 1.0f, 0.0f, 1.0f);
	base_cloud_with_coverage *= CloudCoverage;

	const float height_fraction = get_height_fraction(position);
    return saturate(base_cloud_with_coverage * height_fraction);
}

float henyey_greenstein(float3 userToCloud)
{
    float cosAngle = dot(sunDir, userToCloud);
    float g2 = G * G;
    float threeOverTwo = 3.0f / 2.0f;
    float v = (1.0f + g2 - pow(abs(2.0f * G * cosAngle), threeOverTwo)) * 4.0f * 3.1415f;
    return (1.0f - g2) / v;
}

/*
Sample 6 points 
5 in a cone

1 long distance

cone angle 8
cone size  N
*/
static float3 noise_kernel [] =
{
	float3(-0.8480f, 0.1191f, 0.1354f),
	float3(0.7645f, 0.9764f, -0.9524f),
	float3(0.6827f, -0.5479f, -0.3746f),
	float3(0.8309f, 0.9782f, -0.9803f),
	float3(-0.0018f, -0.3272f, 0.9561f),
	float3(0.2966f, -0.6623f, 0.9219f)
};

float raymarch_cloud_light(float3 position)
{
	const float3 dir = -sunDir;
    const float depth = get_cloud_depth(position, dir);
	const float cone_spread_multiplier = 1.0f;

	const float near_samples_dist = min(5.0f, depth);
	float3 p = position + (dir * near_samples_dist);
	float total_density = 0.0f;

	[unroll]
	for (int i = 0; i < 6; i++)
	{
		p += (cone_spread_multiplier * noise_kernel[i] * float(i));
		total_density += sample_cloud_density(p);
	}
	const float far_sample_dist = min(15.0f, depth);
	total_density += sample_cloud_density(position + (dir * far_sample_dist));

	float powder_sugar_effect = 1.0f - exp(-total_density * 2.0f);
	float beers_law = exp(-total_density * LightAbsorptionTowardSun);
	return 2.0f * beers_law * powder_sugar_effect;
}

float4 cloud_col(float3 dir)
{
    const float inner_shell_raidus = RE_KM + CloudHeightKM;

	const float3 rc = float3(0.0f, -RE_KM, 0.0f);
	const float inner_radius2 = inner_shell_raidus * inner_shell_raidus;

	float tca = dot(rc, dir);
	float d2 = dot(rc, rc) - (tca * tca);
	
	// shouldn't happen
	if (d2 > inner_radius2) return float4(1.0f, 1.0f, 0.0f, 1.0f);

	float thc = sqrt(inner_radius2 - d2);
    float t1 = tca + thc;

	const float3 orig = float3(0.0f, RE_KM, 0.0f) + (dir * t1);
    const float depth = get_cloud_depth(orig, dir);


    float3 light_energy = float3(0.0f, 0.0f, 0.0f);
	float total_density = 0.0f;
	float transmittance = 1.0f;
    
    float hg = henyey_greenstein(dir);

    float dist_travelled = 0.5f * StepSize;
    [loop]
	while (transmittance > 0.05f && dist_travelled < depth)
    {
        float3 ray_pos = orig + (dir * dist_travelled);
        float density = sample_cloud_density(ray_pos) * StepSize;

        float light = raymarch_cloud_light(ray_pos);
        light_energy += light * hg * transmittance * StepSize * 0.5f;

		total_density += density;
		transmittance = exp(-total_density);

        dist_travelled += StepSize;
    }

	float3 colour = light_energy * sunCol;
	// float3 colour = float3(1.0f, 1.0f, 1.0f);

    return float4(colour.r, colour.g, colour.b, 1.0f - transmittance);
}

float4 main(float4 position : SV_Position, float3 normal : Color0, float2 texCoord : TexCoord) : SV_Target
{
	float3 dir = normalize(-normal);
	float4 clouds = cloud_col(dir);
	float3 sky = sky_col(dir);
	float3 blended = (sky * (1.0f - clouds.a)) + (clouds.rgb * clouds.a);
	return float4(blended, 1.0f);
}