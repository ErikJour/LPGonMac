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

};