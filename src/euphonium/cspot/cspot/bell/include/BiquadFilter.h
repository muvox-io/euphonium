#ifndef BELL_BIQUADFILTER_H
#define BELL_BIQUADFILTER_H

#include <mutex>
#include <cmath>
#include "esp_platform.h"

extern "C" int dsps_biquad_f32_ae32(const float* input, float* output, int len, float* coef, float* w);

class BiquadFilter
{
private:
    std::mutex processMutex;
    float coeffs[5];
    float w[2];

public:
    BiquadFilter(){};

    void generateHighShelfCoEffs(float f, float gain, float q)
    {
        if (q <= 0.0001)
        {
            q = 0.0001;
        }
        float Fs = 1;

        float A = sqrtf(pow(10, (double)gain / 20.0));
        float w0 = 2 * M_PI * f / Fs;
        float c = cosf(w0);
        float s = sinf(w0);
        float alpha = s / (2 * q);

        float b0 = A * ((A + 1) + (A - 1) * c + 2 * sqrtf(A) * alpha);
        float b1 = -2 * A * ((A - 1) + (A + 1) * c);
        float b2 = A * ((A + 1) + (A - 1) * c - 2 * sqrtf(A) * alpha);
        float a0 = (A + 1) - (A - 1) * c + 2 * sqrtf(A) * alpha;
        float a1 = 2 * ((A - 1) - (A + 1) * c);
        float a2 = (A + 1) - (A - 1) * c - 2 * sqrtf(A) * alpha;

        std::lock_guard lock(processMutex);
        coeffs[0] = b0 / a0;
        coeffs[1] = b1 / a0;
        coeffs[2] = b2 / a0;
        coeffs[3] = a1 / a0;
        coeffs[4] = a2 / a0;
    }

    // Generates coefficients for a low shelf biquad filter
    void generateLowShelfCoEffs(float f, float gain, float q)
    {
        if (q <= 0.0001)
        {
            q = 0.0001;
        }
        float Fs = 1;

        float A = sqrtf(pow(10, (double)gain / 20.0));
        float w0 = 2 * M_PI * f / Fs;
        float c = cosf(w0);
        float s = sinf(w0);
        float alpha = s / (2 * q);

        float b0 = A * ((A + 1) - (A - 1) * c + 2 * sqrtf(A) * alpha);
        float b1 = 2 * A * ((A - 1) - (A + 1) * c);
        float b2 = A * ((A + 1) - (A - 1) * c - 2 * sqrtf(A) * alpha);
        float a0 = (A + 1) + (A - 1) * c + 2 * sqrtf(A) * alpha;
        float a1 = -2 * ((A - 1) + (A + 1) * c);
        float a2 = (A + 1) + (A - 1) * c - 2 * sqrtf(A) * alpha;

        std::lock_guard lock(processMutex);
        coeffs[0] = b0 / a0;
        coeffs[1] = b1 / a0;
        coeffs[2] = b2 / a0;
        coeffs[3] = a1 / a0;
        coeffs[4] = a2 / a0;
    }

    // Generates coefficients for a notch biquad filter
    void generateNotchCoEffs(float f, float gain, float q)
    {
        if (q <= 0.0001)
        {
            q = 0.0001;
        }
        float Fs = 1;

        float A = sqrtf(pow(10, (double)gain / 20.0));
        float w0 = 2 * M_PI * f / Fs;
        float c = cosf(w0);
        float s = sinf(w0);
        float alpha = s / (2 * q);

        float b0 = 1 + alpha * A;
        float b1 = -2 * c;
        float b2 = 1 - alpha * A;
        float a0 = 1 + alpha;
        float a1 = -2 * c;
        float a2 = 1 - alpha;

        std::scoped_lock lock(processMutex);
        coeffs[0] = b0 / a0;
        coeffs[1] = b1 / a0;
        coeffs[2] = b2 / a0;
        coeffs[3] = a1 / a0;
        coeffs[4] = a2 / a0;
    }

    void processSamples(float *input, int numSamples)
    {
        std::scoped_lock lock(processMutex);

#ifdef ESP_PLATFORM
    dsps_biquad_f32_ae32(input, input, numSamples, coeffs, w);
#else
        // Apply the set coefficients
        for (int i = 0; i < numSamples; i++)
        {
            float d0 = input[i] - coeffs[3] * w[0] - coeffs[4] * w[1];
            input[i] = coeffs[0] * d0 + coeffs[1] * w[0] + coeffs[2] * w[1];
            w[1] = w[0];
            w[0] = d0;
        }
#endif
    }
};

#endif