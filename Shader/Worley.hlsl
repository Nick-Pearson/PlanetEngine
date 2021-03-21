RWTexture2D<float4> Output;
struct WorleyPoint
{
    float2 Position;
};
StructuredBuffer<WorleyPoint> Points;

// Returns the point in a given cell
float2 get_cell_point(uint2 cell)
{
    uint x = cell.x % NUM_CELLS;
    uint y = cell.y % NUM_CELLS;
    float2 noise = Points[x + (y * NUM_CELLS)].Position;
	float2 cell_base = float2(cell.x, cell.y) / NUM_CELLS;
    return cell_base + (noise / NUM_CELLS);
}
// Performs worley noise by checking all adjacent cells
// and comparing the distance to their points
float worley(const float2 coord)
{
    uint2 cell = uint2(coord.x * NUM_CELLS, coord.y * NUM_CELLS) + uint2(NUM_CELLS, NUM_CELLS);
    float dist = 10000000.0f;

    float2 mcoord = coord + float2(1.0f, 1.0f);
    
    // Search in the surrounding cells
    const uint maxX = cell.x + 1;
    const uint maxY = cell.y + 1;
    for (uint x = cell.x - 1; x <= maxX; x++)
    { 
        for (uint y = cell.y - 1; y <= maxY; y++)
        {
        	float2 cell_point = get_cell_point(uint2(x, y));
            dist = min(dist, distance(cell_point, mcoord));
        }
    }
    
    float t2 = 1.0f / (NUM_CELLS * NUM_CELLS);
    dist /= sqrt(t2 + t2);
    return 1.0 - dist;
}

[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID) // Thread ID
{
    float2 uv = threadID.xy / 1024.0f;
    float v = 0.0f;

    float amplitude = 0.5f;
    float frequency = 1.0f;
    
#if OCTAVES <= 1
    v = amplitude * worley(uv);
#else
    for (int i = 0; i < OCTAVES; i++) {
        v += amplitude * worley(frequency * uv);
        frequency *= LACUNARITY;
        amplitude *= GAIN;
    }
#endif

    Output[threadID.xy] = float4(v, v, v, 1.0f);
}