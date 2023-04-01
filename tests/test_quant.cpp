#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

#include "quant.h"

static double rmse(const std::vector<float> &a, const std::vector<float> &b) {
    double s = 0;
    for (size_t i = 0; i < a.size(); i++)
        s += (a[i] - b[i]) * (a[i] - b[i]);
    return std::sqrt(s / a.size());
}

int main() {
    std::vector<float> x(256), y(256);
    for (size_t i = 0; i < x.size(); i++)
        x[i] = std::sin(i * 0.37f) * 2.0f;

    std::vector<block_q8_0> q8(x.size() / QK);
    quantize_q8_0(x.data(), q8.data(), x.size());
    dequantize_q8_0(q8.data(), y.data(), x.size());
    assert(rmse(x, y) < 0.01);

    std::vector<block_q4_0> q4(x.size() / QK);
    quantize_q4_0(x.data(), q4.data(), x.size());
    dequantize_q4_0(q4.data(), y.data(), x.size());
    assert(rmse(x, y) < 0.2);

    // scales this small are subnormal in fp16
    float tiny = 3e-6f;
    assert(std::fabs(fp16_to_fp32(fp32_to_fp16(tiny)) - tiny) < 1e-7f);

    std::vector<block_q4_1> q41(x.size() / QK);
    quantize_q4_1(x.data(), q41.data(), x.size());
    dequantize_q4_1(q41.data(), y.data(), x.size());
    assert(rmse(x, y) < 0.15);

    printf("ok\n");
    return 0;
}
