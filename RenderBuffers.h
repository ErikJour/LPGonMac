//
// Created by Erik Jourgensen on 9/22/26.
//
#include "GameRenderer.h"

#ifndef HOMEMADELPG_RENDERBUFFERS_H
#define HOMEMADELPG_RENDERBUFFERS_H

struct VertexBuffer {
    game_vertex *vertices;
    u32 drawCount;
};

struct GameRenderCommands {
    vertexBuffer *VertexBuffer[3];
    u32 currentFrame;
};

struct MacVertexBuffers
{
	id<MTLBuffer> MetalVertexBuffers[3];
};

#endif //HOMEMADELPG_RENDERBUFFERS_H