#include "quant.h"

#include <algorithm>
#include <cmath>
#include <cstring>


uint16_t fp32_to_fp16(float f) {
    uint32_t x;
    memcpy(&x, &f, 4);
    uint32_t sign = (x >> 16) & 0x8000;
    int32_t exp = ((x >> 23) & 0xFF) - 127 + 15;
    uint32_t mant = x & 0x7FFFFF;
    if (exp >= 31)
        return sign | 0x7C00;
    if (exp <= 0) {
        // subnormal half: shift the mantissa (with the implicit bit) right
        if (exp < -10)
            return sign;
        mant = (mant | 0x800000) >> (1 - exp);
        return sign | (mant >> 13);
    }
    return sign | (exp << 10) | (mant >> 13);
}

float fp16_to_fp32(uint16_t h) {
    uint32_t sign = (h & 0x8000) << 16;
    uint32_t exp = (h >> 10) & 0x1F;
    uint32_t mant = h & 0x3FF;
    uint32_t x;
    if (exp == 0) {
        if (mant == 0) {
            x = sign;
        } else {
            float f = std::ldexp((float)mant, -24);
            return sign ? -f : f;
        }
    } else if (exp == 31) {
        x = sign | 0x7F800000 | (mant << 13);
    } else {
        x = sign | ((exp - 15 + 127) << 23) | (mant << 13);
    }
    float f;
    memcpy(&f, &x, 4);
    return f;
}

void quantize_q8_0(const float *x, block_q8_0 *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float amax = 0;
        for (int j = 0; j < QK; j++)
            amax = std::max(amax, std::fabs(x[i * QK + j]));
        float d = amax / 127.0f;
        float id = d ? 1.0f / d : 0.0f;
        y[i].d = fp32_to_fp16(d);
        for (int j = 0; j < QK; j++)
            y[i].qs[j] = (int8_t)std::lround(x[i * QK + j] * id);
    }
}

void dequantize_q8_0(const block_q8_0 *x, float *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float d = fp16_to_fp32(x[i].d);
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
        y[i].d = fp32_to_fp16(d);
        for (int j = 0; j < QK / 2; j++) {
            int a = std::min(15, (int)std::lround(x[i * QK + j] * id) + 8);
            int b = std::min(15, (int)std::lround(x[i * QK + QK / 2 + j] * id) + 8);
            y[i].qs[j] = a | (b << 4);
        }
    }
}

void dequantize_q4_0(const block_q4_0 *x, float *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float d = fp16_to_fp32(x[i].d);
        for (int j = 0; j < QK / 2; j++) {
            y[i * QK + j] = ((x[i].qs[j] & 0xF) - 8) * d;
            y[i * QK + QK / 2 + j] = ((x[i].qs[j] >> 4) - 8) * d;
        }
    }
}

void quantize_q4_1(const float *x, block_q4_1 *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float mn = x[i * QK], mx = x[i * QK];
        for (int j = 1; j < QK; j++) {
            mn = std::min(mn, x[i * QK + j]);
            mx = std::max(mx, x[i * QK + j]);
        }
        float d = (mx - mn) / 15.0f;
        float id = d ? 1.0f / d : 0.0f;
        y[i].d = fp32_to_fp16(d);
        y[i].m = fp32_to_fp16(mn);
        for (int j = 0; j < QK / 2; j++) {
            int a = std::min(15, (int)std::lround((x[i * QK + j] - mn) * id));
            int b = std::min(15, (int)std::lround((x[i * QK + QK / 2 + j] - mn) * id));
            y[i].qs[j] = a | (b << 4);
        }
    }
}

void dequantize_q4_1(const block_q4_1 *x, float *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float d = fp16_to_fp32(x[i].d), m = fp16_to_fp32(x[i].m);
        for (int j = 0; j < QK / 2; j++) {
            y[i * QK + j] = (x[i].qs[j] & 0xF) * d + m;
            y[i * QK + QK / 2 + j] = (x[i].qs[j] >> 4) * d + m;
        }
    }
}

void quantize_q5_0(const float *x, block_q5_0 *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float amax = 0, max = 0;
        for (int j = 0; j < QK; j++) {
            float v = x[i * QK + j];
            if (std::fabs(v) > amax) {
                amax = std::fabs(v);
                max = v;
            }
        }
        float d = max / -16.0f;
        float id = d ? 1.0f / d : 0.0f;
        y[i].d = fp32_to_fp16(d);
        uint32_t qh = 0;
        for (int j = 0; j < QK / 2; j++) {
            int a = std::min(31, (int)std::lround(x[i * QK + j] * id) + 16);
            int b = std::min(31, (int)std::lround(x[i * QK + QK / 2 + j] * id) + 16);
            y[i].qs[j] = (a & 0xF) | ((b & 0xF) << 4);
            qh |= ((a >> 4) & 1u) << j;
            qh |= ((b >> 4) & 1u) << (j + QK / 2);
        }
        memcpy(y[i].qh, &qh, sizeof qh);
    }
}

void dequantize_q5_0(const block_q5_0 *x, float *y, size_t n) {
    for (size_t i = 0; i < n / QK; i++) {
        float d = fp16_to_fp32(x[i].d);
        uint32_t qh;
        memcpy(&qh, x[i].qh, sizeof qh);
        for (int j = 0; j < QK / 2; j++) {
            int a = (x[i].qs[j] & 0xF) | (((qh >> j) & 1) << 4);
            int b = (x[i].qs[j] >> 4) | (((qh >> (j + QK / 2)) & 1) << 4);
            y[i * QK + j] = (a - 16) * d;
            y[i * QK + QK / 2 + j] = (b - 16) * d;
        }
    }
}
