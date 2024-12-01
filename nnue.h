const int inputSize = 768;

const int l1_size = 32;

const int l2_size = 16;

extern float L1_weights[inputSize][l1_size / 2];

extern float L1_bias[l1_size / 2];

extern float L2_weights[l2_size][l1_size];

extern float L2_bias[l2_size];

extern float output_weights[l2_size];

extern float output_bias;

float relu(float value);

float firstLayer(float (&acc)[2][16], bool perspective);