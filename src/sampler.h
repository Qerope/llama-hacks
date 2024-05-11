#pragma once
#include <cstdint>
#include <random>
#include <vector>

struct SamplerParams {
    float temperature = 0.8f;
    int top_k = 40;
    float top_p = 0.95f;
    float min_p = 0.0f;
    float repeat_penalty = 1.1f;
    int repeat_last_n = 64;
};

int sample(std::vector<float> logits, const SamplerParams &p, std::mt19937 &rng
    , const std::vector<int> &last_tokens = {}
);
