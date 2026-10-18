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
    static constexpr auto relaxed = std::memory_order_relaxed;
    //==================================================================================================================
    //Number of samples the FFT "Hops"
    //==================================================================================================================
    static constexpr int hopSize = fftSize / overlap;
    FFTSetup setup;
    std::array<float, fftSize> window{}, inputFifo{}, frame{};
    std::array<float, fftSize / 2> realP{}, imagP{};
    std::array<std::atomic<float>, numBins> magsDb{};
    int pos{}, count{};

    AnimatedFFT()
    {
        setup = vDSP_create_fftsetup(fftOrder, kFFTRadix2);
        for (int n = 0; n < fftSize; ++n) {
            window[n] = 0.5f * (1.0f - std::cos(2.0f * float(M_PI) * (float)n / fftSize));
        }
        reset();
    }
    ~AnimatedFFT() { if (setup) vDSP_destroy_fftsetup(setup); }
    //==================================================
    //Disable copying
    //=================================================
    AnimatedFFT(const AnimatedFFT&)            = delete;
    AnimatedFFT& operator=(const AnimatedFFT&) = delete;

    void reset()
    {
        pos   = 0;
        count = 0;
        inputFifo.fill(0.0f);
        for (auto& m : magsDb) m.store(-100.0f, relaxed);
    }

    float getBinDb (int k) const { return magsDb[k].load(relaxed); }

    void processFrame()
    {
        //copies raw memory bytes(destination-> beginning of buffer, source -> location in inputFifo , count > bytes to copy)
        std::memcpy(frame.data(), inputFifo.data() + pos, (fftSize - pos) * sizeof(float));
        if (pos > 0) {
            std::memcpy(frame.data() + fftSize - pos, inputFifo.data(), pos * sizeof(float));
        }
        //Tapers audio to 0 at ends of frame
        vDSP_vmul(frame.data(), 1, window.data(), 1, frame.data(), 1, fftSize);
        //Separate the real (cos) and imaginary (sin) parts
        DSPSplitComplex split {realP.data(), imagP.data() };
        vDSP_ctoz((const DSPComplex*) frame.data(), 2, &split, 1, fftSize / 2);
        vDSP_fft_zrip(setup, &split, 1, fftOrder, FFT_FORWARD);

        //Magnitudes
        const float scale = 2.0f / fftSize;
        auto toDb = [](float m) { return 20.0f * std::log10(m + 1e-9f); };
        magsDb[0].store(toDb(std::fabs(realP[0]) * scale * 0.5f), relaxed);
        magsDb[fftSize / 2].store(toDb(std::fabs(imagP[0]) * scale * 0.5f), relaxed);
        for (int k = 1; k < fftSize / 2; ++k) {
            magsDb[k].store(toDb(std::hypot(realP[k], imagP[k]) * scale), relaxed);
        }


    }

};