// shaders.metal
#include <metal_stdlib>
#include "../UI/Common.h"

using namespace metal;

typedef struct {
    float4 position [[position]];
    float4 color;
    float2 uv;
} RasterizerData;

vertex RasterizerData

vertexMain(uint vertexID [[ vertex_id ]],
           constant game_vertex *vertexArray [[ buffer(0) ]])
{
    RasterizerData out;
    float2 pixelSpacePosition = vertexArray[vertexID].position.xy;
    float2 normalizedPosition = (pixelSpacePosition / (512.0 / 2.0)) - 1.0;

    out.position = float4(normalizedPosition.x, normalizedPosition.y, 0.0, 1.0);
    out.color    = vertexArray[vertexID].color;
    out.uv = pixelSpacePosition / 512.0;
    return out;
}

// Plot a line on Y using a value between 0.0-1.0
float plot(vector_float2 st, float pct){
    return smoothstep( pct-0.02, pct, st.y) -
    smoothstep( pct, pct+0.02, st.y);
}

fragment float4 fragmentMain(
    RasterizerData in [[stage_in]],
    constant Uniforms &uniforms [[buffer(11)]])
{
    vector_float2 st    = in.uv;
    float y             = sqrt(st.x);
    vector_float3 color = vector_float3(y);
    float pct           = plot(st, y);
    color               = (1.0-pct)*color+pct*vector_float3(0.0,1.0,0.0);
    return vector_float4(color,1.0);
}