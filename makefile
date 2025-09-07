# Compiler
CXX := g++
CXX_BFLOAT := g++-13
SRC := $(wildcard *.cpp)

# Output binaries
OUT_RELEASE := engine
OUT_DEBUG := engine_debug
OUT_DINT := engine_dint
OUT_DFLOAT := engine_dfloat
OUT_DFLOAT16 := engine_dfloat16
OUT_HCE := engine_hce
OUT_HCE_DATAGEN := engine_hce_datagen
OUT_HCE_DATAGEN2 := engine_hce_datagen2
OUT_HCE_DATAGENMIX := engine_hce_datagen_mix
OUT_NNUE_DATAGEN := nnue_datagen_dfloat

# Common Optimization Flags
CXX_OPTIM := -O3 -march=native -flto=auto -funroll-loops -ffast-math

# Debug Flags
CXX_DEBUG := -O0 -g -Wall -Wextra

# Feature Flags
CXX_HCE := -DHCE
CXX_DATAGEN_SEARCH := -DDATAGEN
CXX_DATAGEN_ENDGAME := -DDATAGEN2

# NNUE Variants
CXX_NNUEINT := -DNNUE -DINT16
CXX_NNUEFLOAT := -DNNUE -DFLOAT -mavx512f
CXX_NNUEFLOAT16 := -DNNUE -DFLOAT16 -mavx512f -mavx512bf16 -std=c++23

# Targets
all: release

release: $(OUT_RELEASE)
debug: $(OUT_DEBUG)
dint: $(OUT_DINT)
dfloat: $(OUT_DFLOAT)
dfloat16: $(OUT_DFLOAT16)
hce: $(OUT_HCE)
hce_datagen: $(OUT_HCE_DATAGEN)
nnue_datagen: $(OUT_NNUE_DATAGEN)

# Build rules
$(OUT_RELEASE): $(SRC)
	$(CXX) $(CXX_OPTIM) $(CXX_HCE) $^ -o $@

$(OUT_DEBUG): $(SRC)
	$(CXX) $(CXX_DEBUG) $^ -o $@

$(OUT_DINT): $(SRC)
	$(CXX) $(CXX_OPTIM) $(CXX_NNUEINT) $^ -o $@

$(OUT_DFLOAT): $(SRC)
	$(CXX) $(CXX_OPTIM) $(CXX_NNUEFLOAT) $^ -o $@

$(OUT_DFLOAT16): $(SRC)
	$(CXX_BFLOAT) $(CXX_OPTIM) $(CXX_NNUEFLOAT16) $^ -o $@

$(OUT_HCE): $(SRC)
	$(CXX) $(CXX_OPTIM) $(CXX_HCE) $^ -o $@

$(OUT_HCE_DATAGEN): $(SRC)
	$(CXX) $(CXX_OPTIM) $(CXX_HCE) $(CXX_DATAGEN_SEARCH) $^ -o $@

$(OUT_HCE_DATAGEN2): $(SRC)
	$(CXX) $(CXX_OPTIM) $(CXX_HCE) $(CXX_DATAGEN_ENDGAME) $^ -o $@

$(OUT_HCE_DATAGENMIX): $(SRC)
	$(CXX) $(CXX_OPTIM) $(CXX_HCE) $(CXX_DATAGEN_SEARCH) $(CXX_DATAGEN_ENDGAME) $^ -o $@

$(OUT_NNUE_DATAGEN): $(SRC)
	$(CXX) $(CXX_OPTIM) $(CXX_NNUEFLOAT) $(CXX_DATAGEN_SEARCH) $^ -o $@

# Clean
clean:
	rm -f $(OUT_RELEASE) $(OUT_DEBUG) $(OUT_DINT) $(OUT_DFLOAT) $(OUT_DFLOAT16) \
		$(OUT_HCE) $(OUT_HCE_DATAGEN)