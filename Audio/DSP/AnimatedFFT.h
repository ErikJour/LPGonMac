//
// Created by Erik Jourgensen on 10/17/26.
//

#pragma once
#include <Accelerate/Accelerate.h>
#include <array>
#include <atomic>
#include <cmath>
#include <cstring>
#include <cstdint>

struct AnimatedFFT {
    //==================================================================================================================
    //The FFT order designates the size of the FFT window and the number of points on which it will operate corresponds
    // to 2 to the power of the order. In this case, an order of 10 which will produce an FFT with 2 ^ 10 = 1024 points.
    //==================================================================================================================
    static constexpr int fftOrder = 10;
    //==================================================================================================================
    //To calculate the FFT size, we use the left bit shift operator which produces 1024 as binary num 10000000000.
    //==================================================================================================================
    static constexpr int fftSize = 1 << fftOrder;
    static constexpr int numBins = fftSize / 2 + 1;
    static constexpr int overlap = 4;
    //==================================================================================================================
    //Number of samples the FFT "Hops"
    //==================================================================================================================
    static constexpr int hopSize = fftSize / overlap;
    FFTSetup setup;
    std::array<float, fftSize> window{}, inputFifo{}, frame{};
    std::array<float, fftSize / 2> realP{}, imageP{};
    std::array<std::atomic<float>, numBins> magsDb{};
    int pos{}, count{};

    AnimatedFFT()
    {
        setup = vDSP_create_fftsetup(fftOrder, kFFTRadix2);
        for (int n = 0; n < fftSize; ++n) {
            window[n] = 0.5f * (1.0f - std::cos(2.0f * float(M_PI) * (float)n / fftSize));
        }
        /*reset();*/
    }
    ~AnimatedFFT() { if (setup) vDSP_destroy_fftsetup(setup); }

};