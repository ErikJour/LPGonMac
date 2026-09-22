//
// Created by Erik Jourgensen on 9/22/26.
//

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

#endif //HOMEMADELPG_RENDERBUFFERS_H