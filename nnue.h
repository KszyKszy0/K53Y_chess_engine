const int inputSize = 768;

const int l1_size = 32;

const int l2_size = 1;

extern int L1_weights[inputSize][l1_size];

extern int L1_bias[l1_size];

extern int L2_weights[l1_size];

extern int output_bias;

int firstLayer(int (&acc)[2][16], bool perspective);