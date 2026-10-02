//
// Created by Erik Jourgensen on 9/30/26.
//

#ifndef HOMEMADELPG_TRIANGLEGEO_H
#define HOMEMADELPG_TRIANGLEGEO_H

#include "../RenderBuffers.h"

void createTriangle (game_vertex *vertices)
{
    float scale = 64.f;

    vertices[0] = { { 232.0f + scale, 232.0f + scale, 0.0f, 1.0f }, {0.0f, 1.0f, 0.0f, 1.0f }  };
    vertices[1] = { { 232.0f + scale, 280.0f + scale, 0.0f, 1.0f }, {0.0f, 0.0f, 1.0f, 1.0f }  };
    vertices[2] = { { 280.0f + scale, 280.0f + scale, 0.0f, 1.0f }, {0.0f, 0.0f, 1.0f, 1.0f }  };

}

#endif //HOMEMADELPG_TRIANGLEGEO_H
