#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct {

    float4 position [[position]];
    float4 color;

} RasterizerData;

struct game_vertex
{
    float4 position;
    float4 color;
};

vertex RasterizerData
vertexMain(uint vertexID [[ vertex_id ]],
                        constant game_vertex *vertexArray [[ buffer(0) ]])
{
    RasterizerData out;
    float2 pixelSpacePosition = vertexArray[vertexID].position.xy;
    out.position              = vector_float4(pixelSpacePosition.x, pixelSpacePosition.y, 0.0, 1.0);
    out.color                 = vertexArray[vertexID].color;

    return out;
}

fragment float4 fragmentMain(RasterizerData in [[stage_in]])
{
return in.color * 0.5;
}