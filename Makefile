CXX ?= g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
BUILD = build

all: $(BUILD)/test_quant
all: $(BUILD)/quant_error
all: $(BUILD)/test_sampler

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/test_quant: tests/test_quant.cpp src/quant.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

$(BUILD)/quant_error: tools/quant_error.cpp src/quant.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

$(BUILD)/test_sampler: tests/test_sampler.cpp src/sampler.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

test: all
	$(BUILD)/test_quant
	$(BUILD)/test_sampler
