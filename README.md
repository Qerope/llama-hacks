# llama-hacks

Small standalone C++ pieces for poking at how llama.cpp works. None of this
loads a model.

- `src/quant.cpp`: q8_0, q4_0, q4_1, q5_0 block quantisation (32 values per
  block, fp16 scale) and the matching dequantisation
- `src/sampler.cpp`: temperature, top-k, top-p, min-p, repetition penalty
- `tools/quant_error`: RMSE of each format on gaussian data
- `tools/kv_estimate`: KV cache size for a given model shape and context
- `tools/gguf_info`: prints the header, metadata keys and tensor list of a
  gguf file (v2 and v3)

    make test
    build/quant_error
    build/kv_estimate 32 4096 8 4096 32768
    build/gguf_info model.gguf
