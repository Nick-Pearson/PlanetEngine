// Determines how many cells there are
#define NUM_CELLS 8.0
#define CHECK_ADJ 1

RWTexture2D<float4> Output;
struct WorleyPoint
{
    float2 Position;
};
StructuredBuffer<WorleyPoint> Points;

// Returns the point in a given cell
float2 get_cell_point(int2 cell)
{
	float2 cell_base = float2(cell) / NUM_CELLS;
    float2 noise = Points[cell.x + (cell.y * NUM_CELLS)].Position;
    return cell_base + (noise / NUM_CELLS);
}

// Performs worley noise by checking all adjacent cells
// and comparing the distance to their points
float worley(float2 coord)
{
    int2 cell = int2(coord * NUM_CELLS);
    float dist = 1.0f;
    
    // Search in the surrounding cells
    float maxX = cell.x + CHECK_ADJ;
    float maxY = cell.y + CHECK_ADJ;
    for (int x = cell.x - CHECK_ADJ; x <= maxX; x++)
    { 
        for (int y = cell.y - CHECK_ADJ; y <= maxY; y++)
        {
        	float2 cell_point = get_cell_point(int2(x, y));
            dist = min(dist, distance(cell_point, coord));
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
    float v = worley(uv);
    Output[threadID.xy] = float4(v, v, v, 1.0f);
}