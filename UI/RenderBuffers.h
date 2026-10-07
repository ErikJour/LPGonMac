//
// Created by Erik Jourgensen on 9/22/26.
//
#ifndef HOMEMADELPG_RENDERBUFFERS_H
#define HOMEMADELPG_RENDERBUFFERS_H
#include "GameRenderer.h"


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

#endif //HOMEMADELPG_RENDERBUFFERS_H