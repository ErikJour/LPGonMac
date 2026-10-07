//
// Created by Erik Jourgensen on 9/30/26.
//

#ifndef HOMEMADELPG_SQUAREGEO_H
#define HOMEMADELPG_SQUAREGEO_H

#include "../UI/RenderBuffers.h"
#include "../UI/mtkViewDelegate.h"

void drawSquare (game_vertex *vertices, uint32_t *vertexCount, uint32_t side, vector_float4 color, uint32_t offsetX, uint32_t offsetY)
{
    float screenWidth = 512.;
    float centerX     = 512/2 + offsetX;
    float centerY     = 512/2 + offsetY;
    float minX        = centerX - side;
    float maxX        = centerX + side;
    float minY        = centerY - side;
    float maxY        = centerY + side;

    game_vertex v1 = {{minX, minY, 0.0f, 1.0f }, color };
    vertices[(*vertexCount)++] = v1;
    game_vertex v2 = {{minX, maxY, 0.0f, 1.0f }, color  };
    vertices[(*vertexCount)++] = v2;
    game_vertex v3 = {{maxX, maxY, 0.0f, 1.0f }, color  };
    vertices[(*vertexCount)++] = v3;
    game_vertex v4 = {{minX, minY, 0.0f, 1.0f }, color };
    vertices[(*vertexCount)++] = v4;
    game_vertex v5 = {{maxX, minY, 0.0f, 1.0f }, color  };
    vertices[(*vertexCount)++] = v5;
    game_vertex v6 = {{maxX, maxY, 0.0f, 1.0f }, color  };
    vertices[(*vertexCount)++] = v6;
}

#endif //HOMEMADELPG_SQUAREGEO_H
