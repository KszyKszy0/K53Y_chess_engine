#include "nnue.h"
#include "enums.h"


int firstLayer(int (&acc)[2][16], bool perspective)
{
    int result = 0;
    if(perspective == WHITE)
    {
        for(int i=0; i < 16; i++)
        {
            result += acc[0][i] * L2_weights[i];
            result += acc[1][i] * L2_weights[i+16];
        }
    }else
    {
        for(int i=0; i < 16; i++)
        {
            result += acc[1][i] * L2_weights[i];
            result += acc[0][i] * L2_weights[i+16];
        }
    }
    return result;
}