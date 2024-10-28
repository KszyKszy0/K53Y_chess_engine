#include "nnue.h"
#include "enums.h"
#include <immintrin.h>

float relu(float value)
{
    if(value < 0)
    {
        return (value / 10.f);
    }
    return value;
}

__m256 relu_avx(__m256 x) {
    return _mm256_max_ps(x, _mm256_setzero_ps());
}

float firstLayer(float (&acc)[2][16], bool perspective)
{
    float result = 0;
    float hidden[l2_size] = {0};
    if(perspective == WHITE)
    {
        // for(int i=0; i < l2_size; i++)
        // {
        //     for(int j=0; j < (l1_size / 2); j++)
        //     {
        //         hidden[i] += relu(acc[0][j] + L1_bias[j]) * L2_weights[i][j];
        //         hidden[i] += relu(acc[1][j] + L1_bias[j+16]) * L2_weights[i][j+16];
        //     }
        // }
        for(int i=0; i < l2_size; i++)
        {
            for(int j=0; j < (l1_size / 2); j++)
            {
                hidden[i] += relu(acc[0][j] + L1_bias[j]) * L2_weights[i][j];
                hidden[i] += relu(acc[1][j] + L1_bias[j+16]) * L2_weights[i][j+16];
            }
        }
        for(int i=0; i < l2_size; i++)
        {
            result += relu(hidden[i] + L2_bias[i]) * output_weights[i];
        }

    }else
    {
        for(int i=0; i < l2_size; i++)
        {
            for(int j=0; j < (l1_size / 2); j++)
            {
                hidden[i] += relu(acc[1][j] + L1_bias[j]) * L2_weights[i][j];
                hidden[i] += relu(acc[0][j] + L1_bias[j+16]) * L2_weights[i][j+16];
            }
        }
        for(int i=0; i < l2_size; i++)
        {
            result += relu(hidden[i] + L2_bias[i]) * output_weights[i];
        }
        // result += output_bias;
    }
    return (result + output_bias)*100;
}

