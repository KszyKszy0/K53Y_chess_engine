#define INPUT_SIZE 768

#define L1_SIZE 512

#define L2_SIZE 32

extern float L1_weights[INPUT_SIZE][L1_SIZE / 2];

extern float L1_bias[L1_SIZE / 2];

extern float L2_weights[L2_SIZE][L1_SIZE];

extern float L2_bias[L2_SIZE];

extern float output_weights[L2_SIZE];

extern float output_bias;

float relu(float value);

float firstLayer(float (&acc)[L1_SIZE], bool perspective);