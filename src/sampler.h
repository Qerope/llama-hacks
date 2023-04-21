#pragma once
#include <cstdint>
#include <random>
#include <vector>

struct SamplerParams {
    float temperature = 0.8f;
    int top_k = 40;
};

int sample(std::vector<float> logits, const SamplerParams &p, std::mt19937 &rng
);
