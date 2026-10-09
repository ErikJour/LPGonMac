#include <metal_stdlib>
#include <simd/simd.h>
#include "../UI/GameRenderer.h"
#include "../UI/Common.h"

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
    float2 normalizedPosition = (pixelSpacePosition / (512. / 2.0)) - 1;

    out.position              = vector_float4(normalizedPosition.x, normalizedPosition.y, 0.0, 1.0);
    out.color                 = vertexArray[vertexID].color;

    return out;
}

fragment float4 fragmentMain(
        RasterizerData in [[stage_in]],
        constant Uniforms &uniforms [[buffer(11)]])
{
return in.color * 0.5 /** uniforms.audioData*/;
}