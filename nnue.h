const int inputSize = 768;

const int l1_size = 32;

const int l2_size = 1;

extern double L1_weights[inputSize][l1_size];

extern double L1_bias[l1_size];

extern double L2_weights[l1_size];

extern double output_bias;

double relu(double value);

double firstLayer(double (&acc)[2][16], bool perspective);