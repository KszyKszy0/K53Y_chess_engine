#include "nnue.h"
#include "enums.h"

double relu(double value)
{
    if(value < 0)
    {
        return (value / 10.f);
    }
    return value;
}

double firstLayer(double (&acc)[2][16], bool perspective)
{
    double result = 0;
    double hidden[l2_size] = {0};
    if(perspective == WHITE)
    {
        // for(int i=0; i < 16; i++)
        // {
        //     result += relu(acc[0][i] + L1_bias[i]) * L2_weights[i];
        //     result += relu(acc[1][i] + L1_bias[i+16]) * L2_weights[i+16];
        // }
        for(int i=0; i < l2_size; i++)
        {
            for(int j=0; j < (l1_size / 2); j++)
            {
                hidden[i] += relu(acc[0][j] + L1_bias[j]) * L2_weights[j][i];
                hidden[i] += relu(acc[1][j] + L1_bias[j+16]) * L2_weights[j+16][i];
            }
        }
        for(int i=0; i < l2_size; i++)
        {
            result += relu(hidden[i]) * output_weights[i];
        }
        // result += output_bias;
    }else
    {
        for(int i=0; i < l2_size; i++)
        {
            for(int j=0; j < (l1_size / 2); j++)
            {
                hidden[i] += relu(acc[1][j] + L1_bias[j]) * L2_weights[j][i];
                hidden[i] += relu(acc[0][j] + L1_bias[j+16]) * L2_weights[j+16][i];
            }
        }
        for(int i=0; i < l2_size; i++)
        {
            result += relu(hidden[i]) * output_weights[i];
        }
        // result += output_bias;
    }
    return (result + output_bias)*100;
}

