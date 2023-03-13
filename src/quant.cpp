#include "quant.h"

#include <algorithm>
#include <cmath>


void quantize_q8_0(const float *x, block_q8_0 *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float amax = 0;
        for (int j = 0; j < QK; j++)
            amax = std::max(amax, std::fabs(x[i * QK + j]));
        float d = amax / 127.0f;
        float id = d ? 1.0f / d : 0.0f;
        y[i].d = d;
        for (int j = 0; j < QK; j++)
            y[i].qs[j] = (int8_t)std::lround(x[i * QK + j] * id);
    }
}

void dequantize_q8_0(const block_q8_0 *x, float *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float d = x[i].d;
        for (int j = 0; j < QK; j++)
            y[i * QK + j] = x[i].qs[j] * d;
    }
}

void quantize_q4_0(const float *x, block_q4_0 *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float amax = 0, max = 0;
        for (int j = 0; j < QK; j++) {
            float v = x[i * QK + j];
            if (std::fabs(v) > amax) {
                amax = std::fabs(v);
                max = v;
            }
        }
        // map the signed max to -8 so the full range of 4 bits is used
        float d = max / -8.0f;
        float id = d ? 1.0f / d : 0.0f;
        y[i].d = d;
        for (int j = 0; j < QK / 2; j++) {
            int a = std::min(15, (int)std::lround(x[i * QK + j] * id) + 8);
            int b = std::min(15, (int)std::lround(x[i * QK + QK / 2 + j] * id) + 8);
            y[i].qs[j] = a | (b << 4);
        }
    }
}

void dequantize_q4_0(const block_q4_0 *x, float *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float d = x[i].d;
        for (int j = 0; j < QK / 2; j++) {
            y[i * QK + j] = ((x[i].qs[j] & 0xF) - 8) * d;
            y[i * QK + QK / 2 + j] = ((x[i].qs[j] >> 4) - 8) * d;
        }
    }
}
