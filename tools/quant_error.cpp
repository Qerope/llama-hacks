#include <cmath>
#include <cstdio>
#include <random>
#include <vector>

#include "quant.h"

template <typename Block, typename Q, typename DQ>
static double measure(const std::vector<float> &x, Q q, DQ dq) {
    std::vector<Block> blocks(x.size() / QK);
    std::vector<float> y(x.size());
    q(x.data(), blocks.data(), x.size());
    dq(blocks.data(), y.data(), x.size());
    double s = 0;
    for (size_t i = 0; i < x.size(); i++)
        s += (x[i] - y[i]) * (x[i] - y[i]);
    return std::sqrt(s / x.size());
}

int main() {
    std::mt19937 rng(42);
    std::normal_distribution<float> dist(0.0f, 1.0f);
    std::vector<float> x(1 << 20);
    for (auto &v : x)
        v = dist(rng);

    printf("q8_0  %.5f\n", measure<block_q8_0>(x, quantize_q8_0, dequantize_q8_0));
    printf("q4_0  %.5f\n", measure<block_q4_0>(x, quantize_q4_0, dequantize_q4_0));
    printf("q4_1  %.5f\n", measure<block_q4_1>(x, quantize_q4_1, dequantize_q4_1));
    return 0;
}
