#pragma once
#include <vector>

// Given the target model's greedy choice at each draft position, return how
// many leading draft tokens are accepted.
inline size_t accepted_prefix(const std::vector<int> &draft, const std::vector<int> &target) {
    size_t n = 0;
    while (n < draft.size() && n < target.size() && draft[n] == target[n])
        n++;
    return n;
}
