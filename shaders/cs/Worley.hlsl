RWTexture3D<float4> Output;
struct WorleyPoint
{
    float3 Position;
    float padding;
};
StructuredBuffer<WorleyPoint> Points;

// Returns the point in a given cell
float3 get_cell_point(uint3 cell, int num_cells)
{
    uint x = cell.x % num_cells;
    uint y = cell.y % num_cells;
    uint z = cell.z % num_cells;
    float3 noise = Points[x + (y * num_cells) + (z * num_cells * num_cells)].Position;
	float3 cell_base = float3(cell.x, cell.y, cell.z) / num_cells;
    return cell_base + (noise / num_cells);
}
// Performs worley noise by checking all adjacent cells
// and comparing the distance to their points
float worley(const float3 coord, int num_cells)
{
    uint3 cell = uint3(coord * num_cells) + uint3(num_cells, num_cells, num_cells);
    float dist = 1.0f;

    float3 mcoord = coord + float3(1.0f, 1.0f, 1.0f);
    
    // Search in the surrounding cells
    for (uint x = cell.x - 1; x <= cell.x + 1; x++)
    { 
        for (uint y = cell.y - 1; y <= cell.y + 1; y++)
        {
            for (uint z = cell.z - 1; z <= cell.z + 1; z++)
            {
        	    float3 cell_point = get_cell_point(uint3(x, y, z), num_cells);
                dist = min(dist, distance(cell_point, mcoord));
            }
        }
    }
    
    float t2 = 1.0f / (num_cells * num_cells);
    dist /= sqrt(t2 + t2);
    float g = 1.0f - dist;
	return max(0.0f, (g - 0.1f) * 1.1f);
}

// -----------------------------------------------
// -----------------------------------------------

/* discontinuous pseudorandom uniformly distributed in [-0.5, +0.5]^3 */
float3 random3(float3 c) {
	float j = 4096.0*sin(dot(c,float3(17.0, 59.4, 15.0)));
	float3 r;
	r.z = frac(512.0*j);
	j *= .125;
	r.x = frac(512.0*j);
	j *= .125;
	r.y = frac(512.0*j);
	return r-0.5;
}

/* skew constants for 3d simplex functions */
static const float F3 =  0.3333333f;
static const float G3 =  0.1666667f;

float simplex(float3 uv)
{
    /*
    double c=4, a=1; // torus parameters (controlling size)
    double xt = (c+a*cos(2*PI*y))*cos(2*PI*x);
    double yt = (c+a*cos(2*PI*y))*sin(2*PI*x);
    double zt = a*sin(2*PI*y);
    double val = PerlinNoise3D( xt,yt,zt, 1.5, 2, 12 ) ; // torus
    */


    /* 1. find current tetrahedron T and it's four vertices */
    /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
    /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/
    
    /* calculate s and x */
    float3 s = floor(uv + dot(uv, float3(F3, F3, F3)));
    float3 x = uv - s + dot(s, float3(G3, G3, G3));
    
    /* calculate i1 and i2 */
    float3 e = step(float3(0.0f, 0.0f, 0.0f), x - x.yzx);
    float3 i1 = e*(1.0 - e.zxy);
    float3 i2 = 1.0 - e.zxy*(1.0 - e);
    
    /* x1, x2, x3 */
    float3 x1 = x - i1 + G3;
    float3 x2 = x - i2 + 2.0*G3;
    float3 x3 = x - 1.0 + 3.0*G3;
    
    /* 2. find four surflets and store them in d */
    float4 w, d;
    
    /* calculate surflet weights */
    w.x = dot(x, x);
    w.y = dot(x1, x1);
    w.z = dot(x2, x2);
    w.w = dot(x3, x3);
    
    /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
    w = max(0.6 - w, 0.0);
    
    /* calculate surflet components */
    d.x = dot(random3(s), x);
    d.y = dot(random3(s + i1), x1);
    d.z = dot(random3(s + i2), x2);
    d.w = dot(random3(s + 1.0), x3);
    
    /* multiply d by w^4 */
    w *= w;
    w *= w;
    d *= w;
    
    /* 3. return the sum of the four surflets */
    return dot(d, float4(52.0f, 52.0f, 52.0f, 52.0f));
}

float simplex_fbm(float3 uv)
{
    float amplitude = 0.5f;
    float frequency = 3.0f;
	
    // float2x2 m = float2x2(1.6f, 1.2f,
    //                     -1.2f,  1.6f);

	float v = 0.0f;
#if OCTAVES <= 1
    v = amplitude * simplex(frequency * uv);
#else
    for (int i = 0; i < 7; i++)
    {
        v += amplitude * simplex(frequency * uv);
        frequency *= LACUNARITY;
        amplitude *= GAIN;
    }
#endif 

    // v range -1,1
    // v range -3,3
    v = v * 3.0f;
    // v range -2,4
    v = v + 1.0f;
    // v range 0,4
    v = abs(v);
    // v range 0,1
	return v / 4.0f;
}

float worley_fbm(float3 uv, float amplitude, int num_cells)
{    
    float frequency = 1.0f;

#if OCTAVES <= 1
    return worley(frequency * uv, num_cells);
#else
    float v = 0.0f;
    for (int i = 0; i < OCTAVES; i++)
    {
        v += amplitude * worley(frequency * uv, num_cells);
        frequency *= LACUNARITY;
        amplitude *= GAIN;
    }
    return v;
#endif
}

float remap(float originalValue, float originalMin, float originalMax, float newMin, float newMax)
{
	return newMin + (((originalValue - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

[numthreads(THREAD_GROUP_SIZE, THREAD_GROUP_SIZE, THREAD_GROUP_SIZE)]
void main(uint3 threadID : SV_DispatchThreadID) // Thread ID
{
    float3 uv = threadID.xyz / (float)(TEXTURE_WIDTH);

#if INCLUDE_SIMPLEX
    float w = worley_fbm(uv, 0.4f, NUM_CELLS - 3);
    float p = simplex_fbm(uv);
    float r = remap(p, 0.0f, 1.0f, w - 0.2f, 1.0f);
#else
    float r = worley_fbm(uv, 0.7f,NUM_CELLS - 3);
#endif

    float g = worley_fbm(uv, 0.6f, NUM_CELLS - 2);
    float b = worley_fbm(uv, 0.7f, NUM_CELLS - 1);
    float a = worley_fbm(uv, 0.7f, NUM_CELLS);

    Output[threadID.xyz] = float4(r, g, b, a);
}