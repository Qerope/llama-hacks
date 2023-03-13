#pragma once
#include <cstddef>
#include <cstdint>

constexpr int QK = 32;

struct block_q8_0 {
    float d;
    int8_t qs[QK];
};

void quantize_q8_0(const float *x, block_q8_0 *y, size_t n);
void dequantize_q8_0(const block_q8_0 *x, float *y, size_t n);
