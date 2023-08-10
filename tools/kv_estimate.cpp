#include <cstdio>
#include <cstdlib>

// usage: kv_estimate N_LAYER N_EMBD N_HEAD N_CTX
int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "usage: %s N_LAYER N_EMBD N_HEAD N_CTX\n", argv[0]);
        return 2;
    }
    double n_layer = atof(argv[1]), n_embd = atof(argv[2]), n_head = atof(argv[3]), n_ctx = atof(argv[4]);
    // K and V, fp16 each
    double bytes = 2 * n_layer * n_ctx * n_embd * 2;
    printf("%.1f MiB\n", bytes / (1024 * 1024));
    return 0;
}
