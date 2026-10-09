//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once
#include <iostream>
#include "simd/simd.h"

typedef struct {

    float audioData;

} Uniforms;


struct Colors {

    vector_float4 blue    = { 0.0f, 0.0f, 1.0f, 1.0f };
    vector_float4 red     = { 1.0f, 0.0f, 0.0f, 1.0f };
    vector_float4 green   = { 0.0f, 1.0f, 0.0f, 1.0f };
    vector_float4 cream   = { 0.5f, 1.0f, 1.0f, 1.0f };

};

struct game_vertex
{
    vector_float4 position;
    vector_float4 color;
};

struct VertexBuffer {
    game_vertex *vertices;
    uint32_t     drawCount;
};

struct GameRenderCommands {
    VertexBuffer *vertexBuffer[3];
    uint32_t      currentFrame;
};

struct MacVertexBuffers
{
    id<MTLBuffer> MetalVertexBuffers[3];
};