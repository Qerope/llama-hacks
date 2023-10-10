#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

static FILE *f;

template <typename T>
static T rd() {
    T v;
    if (fread(&v, sizeof(T), 1, f) != 1) {
        fprintf(stderr, "unexpected end of file\n");
        exit(1);
    }
    return v;
}

static std::string rd_str() {
    uint32_t n = rd<uint32_t>();
    std::string s(n, '\0');
    if (n && fread(&s[0], 1, n, f) != n)
        exit(1);
    return s;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s file.gguf\n", argv[0]);
        return 2;
    }
    f = fopen(argv[1], "rb");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    char magic[4];
    if (fread(magic, 1, 4, f) != 4 || memcmp(magic, "GGUF", 4) != 0) {
        fprintf(stderr, "not a gguf file\n");
        return 1;
    }
    uint32_t version = rd<uint32_t>();
    uint32_t n_tensors = rd<uint32_t>();
    uint32_t n_kv = rd<uint32_t>();
    printf("version %u, %llu tensors, %llu metadata keys\n", version,
           (unsigned long long)n_tensors, (unsigned long long)n_kv);
    for (uint64_t i = 0; i < n_kv; i++) {
        std::string key = rd_str();
        uint32_t type = rd<uint32_t>();
        printf("  %s (type %u)\n", key.c_str(), type);
        break;
    }
    return 0;
}
