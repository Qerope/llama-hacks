#include "sampler.h"

#include <algorithm>
#include <cmath>
#include <numeric>

int sample(std::vector<float> logits, const SamplerParams &p, std::mt19937 &rng
    , const std::vector<int> &last_tokens
) {
    int n = std::min<int>(p.repeat_last_n, last_tokens.size());
    for (int i = 0; i < n; i++) {
        int t = last_tokens[last_tokens.size() - 1 - i];
        logits[t] = logits[t] > 0 ? logits[t] / p.repeat_penalty : logits[t] * p.repeat_penalty;
    }
    if (p.temperature <= 0) {
        return std::max_element(logits.begin(), logits.end()) - logits.begin();
    }
    for (auto &l : logits)
        l /= p.temperature;

    std::vector<int> idx(logits.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&](int a, int b) { return logits[a] > logits[b]; });
    if (p.top_k > 0 && (size_t)p.top_k < idx.size())
        idx.resize(p.top_k);

    std::vector<float> probs(idx.size());
    float mx = logits[idx[0]], sum = 0;
    for (size_t i = 0; i < idx.size(); i++) {
        probs[i] = std::exp(logits[idx[i]] - mx);
        sum += probs[i];
    }
    for (auto &q : probs)
        q /= sum;

    float cum = 0;
    size_t keep = probs.size();
    for (size_t i = 0; i < probs.size(); i++) {
        cum += probs[i];
        if (cum >= p.top_p) {
            keep = i + 1;
            break;
        }
    }
    idx.resize(keep);
    probs.resize(keep);

    std::discrete_distribution<int> d(probs.begin(), probs.end());
    return idx[d(rng)];
}
