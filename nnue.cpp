#include "nnue.h"
#include "enums.h"

double relu(double value)
{
    if(value < 0)
    {
        return 0;
    }
    return value;
}

double firstLayer(double (&acc)[2][16], bool perspective)
{
    double result = 0;
    if(perspective == WHITE)
    {
        for(int i=0; i < 16; i++)
        {
            result += relu(acc[0][i] + L1_bias[i]) * L2_weights[i];
            result += relu(acc[1][i] + L1_bias[i+16]) * L2_weights[i+16];
        }
    }else
    {
        for(int i=0; i < 16; i++)
        {
            result += relu(acc[1][i] + L1_bias[i]) * L2_weights[i];
            result += relu(acc[0][i] + L1_bias[i+16]) * L2_weights[i+16];
        }
    }
    return result + output_bias;
}

