#pragma once
#include <cstddef>
#include <cstdint>

constexpr int QK = 32;

struct block_q8_0 {
    uint16_t d;
    int8_t qs[QK];
};

void quantize_q8_0(const float *x, block_q8_0 *y, size_t n);
void dequantize_q8_0(const block_q8_0 *x, float *y, size_t n);

struct block_q4_0 {
    uint16_t d;
    uint8_t qs[QK / 2];
};

void quantize_q4_0(const float *x, block_q4_0 *y, size_t n);
void dequantize_q4_0(const block_q4_0 *x, float *y, size_t n);

uint16_t fp32_to_fp16(float f);
float fp16_to_fp32(uint16_t h);

struct block_q4_1 {
    uint16_t d;
    uint16_t m;
    uint8_t qs[QK / 2];
};

void quantize_q4_1(const float *x, block_q4_1 *y, size_t n);
void dequantize_q4_1(const block_q4_1 *x, float *y, size_t n);
