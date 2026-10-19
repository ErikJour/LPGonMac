//
// Created by Erik Jourgensen on 10/19/26.
//

#pragma once

#include <cstdint>

class AnimatedNoise
{
public:
    explicit AnimatedNoise (uint32_t seed = 0x9E3779B9u) : state (seed ? seed : 1u) {}


     void nextFloat(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {
            int frame = 0;

             auto* leftChannel  = static_cast<Float32*> (ioData->mBuffers[0].mData);
             auto* rightChannel = static_cast<Float32*> (ioData->mBuffers[1].mData);

            for (frame = 0; frame < inNumberFrames; ++frame) {
                    auto noise = nextBipolar();
                    (leftChannel)[frame]  += noise;
                    (rightChannel)[frame] += noise;
            }
    }


private:
    uint32_t next() noexcept
    {
            // xorshift32 (Marsaglia); state must never be 0
            state ^= state << 13;
            state ^= state >> 17;
            state ^= state << 5;
            return state;
    }

    float nextBipolar() noexcept
    {
            return static_cast<int32_t> (next()) * (1.0f / 2147483648.0f); // [-1, 1)
    }

    uint32_t state;
};