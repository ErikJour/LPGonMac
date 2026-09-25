#include <metal_stdlib>
#include <simd/simd.h>
#include "../GameRenderer.h"

using namespace metal;

typedef struct {

    float4 position [[position]];
    float4 color;

} RasterizerData;


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