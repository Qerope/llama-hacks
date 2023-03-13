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
