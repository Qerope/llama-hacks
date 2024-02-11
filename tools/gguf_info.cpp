#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static const uint8_t *cur;

template <typename T>
static T rd() {
    T v;
    memcpy(&v, cur, sizeof(T));
    cur += sizeof(T);
    return v;
}

static std::string rd_str() {
    uint64_t n = rd<uint64_t>();
    std::string s((const char *)cur, n);
    cur += n;
    return s;
}
    uint64_t n = rd<uint64_t>();

static void skip_value(uint32_t type);

static void skip_value(uint32_t type) {
    static const int sizes[] = {1, 1, 2, 2, 4, 4, 4, 1, 0, 0, 8, 8, 8};
    if (type == 8) {
        rd_str();
    } else if (type == 9) {
        uint32_t et = rd<uint32_t>();
        uint64_t n = rd<uint64_t>();
        for (uint64_t i = 0; i < n; i++)
            skip_value(et);
    } else if (type < 13) {
        cur += sizes[type];
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s file.gguf\n", argv[0]);
        return 2;
    }
    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    if (fd < 0 || fstat(fd, &st) != 0) {
        perror(argv[1]);
        return 1;
    }
    const uint8_t *base = (const uint8_t *)mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (base == MAP_FAILED || memcmp(base, "GGUF", 4) != 0) {
        fprintf(stderr, "not a gguf file\n");
        return 1;
    }
    cur = base + 4;
    uint32_t version = rd<uint32_t>();
    uint64_t n_tensors = rd<uint64_t>();
    uint64_t n_kv = rd<uint64_t>();
    printf("version %u, %llu tensors, %llu metadata keys\n", version,
           (unsigned long long)n_tensors, (unsigned long long)n_kv);
    for (uint64_t i = 0; i < n_kv; i++) {
        std::string key = rd_str();
        uint32_t type = rd<uint32_t>();
        printf("  %s (type %u)\n", key.c_str(), type);
        skip_value(type);
    }
    static const char *type_names[] = {"f32", "f16", "q4_0", "q4_1", "q4_2", "q4_3", "q5_0", "q5_1", "q8_0"};
    for (uint64_t i = 0; i < n_tensors; i++) {
        std::string name = rd_str();
        uint32_t ndim = rd<uint32_t>();
        printf("%s [", name.c_str());
        for (uint32_t d = 0; d < ndim; d++)
            printf("%s%llu", d ? ", " : "", (unsigned long long)rd<uint64_t>());
        uint32_t type = rd<uint32_t>();
        rd<uint64_t>();  // offset
        printf("] %s\n", type < 9 ? type_names[type] : "?");
    }
    return 0;
}
