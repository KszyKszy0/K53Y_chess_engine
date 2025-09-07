#include <cstdint>
#include "accumulator.h"
#include "consts.h"
#include <stdfloat>

#ifdef INT16
alignas(32) extern int16_t L1_weights[INPUT_SIZE][L1_SIZE / 2];

alignas(32) extern int16_t L1_bias[L1_SIZE / 2];

alignas(32) extern int16_t L2_weights[L2_SIZE][L1_SIZE];

alignas(32) extern int16_t L2_bias[L2_SIZE];

alignas(32) extern int16_t output_weights[L2_SIZE];

alignas(32) extern int16_t output_bias;
#endif 

#ifdef FLOAT
alignas(32) extern float L1_weights[INPUT_SIZE][L1_SIZE / 2];

alignas(32) extern float L1_bias[L1_SIZE / 2];

alignas(32) extern float L2_weights[L2_SIZE][L1_SIZE];

alignas(32) extern float L2_bias[L2_SIZE];

alignas(32) extern float output_weights[L2_SIZE];

alignas(32) extern float output_bias;
#endif

#ifdef FLOAT16
alignas(32) extern float L1_weights[INPUT_SIZE][L1_SIZE / 2];

alignas(32) extern float L1_bias[L1_SIZE / 2];

alignas(64) extern std::bfloat16_t L2_weights[L2_SIZE][L1_SIZE];

alignas(32) extern float L2_bias[L2_SIZE];

alignas(32) extern float output_weights[L2_SIZE];

alignas(32) extern float output_bias;
#endif

float relu(float value);

float firstLayer(Accumulator& acc, bool perspective);