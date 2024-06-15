#include <cassert>
#include <cstdio>

#include "sampler.h"
#include "speculative.h"

int main() {
    std::mt19937 rng(1);
    std::vector<float> logits = {0.1f, 2.0f, 0.5f, 3.0f, -1.0f};

    SamplerParams greedy;
    greedy.temperature = 0;
    assert(sample(logits, greedy, rng) == 3);

    SamplerParams k1;
    k1.top_k = 1;
    for (int i = 0; i < 20; i++)
        assert(sample(logits, k1, rng) == 3);

    SamplerParams p;
    p.top_k = 0;
    p.top_p = 0.5f;
    for (int i = 0; i < 20; i++)
        assert(sample(logits, p, rng) == 3);

    // a repeated token is pushed down, so greedy no longer picks it
    SamplerParams rep;
    rep.temperature = 0;
    rep.repeat_penalty = 3.0f;
    assert(sample(logits, rep, rng, {3}) == 1);

    SamplerParams mp;
    mp.top_k = 0;
    mp.top_p = 1.0f;
    mp.min_p = 0.5f;
    for (int i = 0; i < 20; i++) {
        int t = sample(logits, mp, rng);
        assert(t == 3 || t == 1);
    }

    assert(accepted_prefix({1, 2, 3}, {1, 2, 9}) == 2);
    assert(accepted_prefix({}, {1}) == 0);

    printf("ok\n");
    return 0;
}
