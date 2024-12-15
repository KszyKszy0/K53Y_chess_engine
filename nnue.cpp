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

#ifdef NNUE
__m256 relu_avx(__m256 x) {
    return _mm256_max_ps(x, _mm256_setzero_ps());
}


// Funkcja pomocnicza do sumowania elementów wektora AVX
inline float _mm256_reduce_add_ps(__m256 &vec) {
    __m256 hsum = _mm256_hadd_ps(vec, vec);
    hsum = _mm256_hadd_ps(hsum, hsum);
    __m128 hsum_low = _mm256_castps256_ps128(hsum);
    __m128 hsum_high = _mm256_extractf128_ps(hsum, 1);
    hsum_low = _mm_add_ps(hsum_low, hsum_high);
    return _mm_cvtss_f32(hsum_low);
}


float firstLayer(float (&acc)[2][16], bool perspective)
{
    float result = 0;
    float hidden[l2_size] = {0};
    if(perspective == WHITE)
    {

        __m256 zero = _mm256_setzero_ps(); // ReLU – dla wartości poniżej 0
        __m256 leaky_scale = _mm256_set1_ps(0.1f);
        // Pierwsza pętla: wyliczanie `hidden`
        for (int i = 0; i < l2_size; i++) {
            __m256 hidden_vec = _mm256_setzero_ps();

            for (int j = 0; j < l1_size / 2; j += 8) {
                // Ładujemy 8-elementowe fragmenty acc[0][j] i L1_bias[j] oraz liczymy pierwszą część hidden
                __m256 acc_vec_0 = _mm256_loadu_ps(&acc[0][j]);
                __m256 bias_vec_0 = _mm256_loadu_ps(&L1_bias[j]);
                __m256 weighted_sum_0 = _mm256_add_ps(acc_vec_0, bias_vec_0);

                // Leaky ReLU dla `weighted_sum_0`
                // __m256 mask_0 = _mm256_cmp_ps(weighted_sum_0, zero, _CMP_GT_OS);  // Maskowanie
                // __m256 leaky_relu_0 = _mm256_blendv_ps(_mm256_mul_ps(weighted_sum_0, leaky_scale), weighted_sum_0, mask_0);
                weighted_sum_0 = _mm256_max_ps(weighted_sum_0, zero); // ReLU

                __m256 weights_vec_0 = _mm256_loadu_ps(&L2_weights[i][j]);
                hidden_vec = _mm256_fmadd_ps(weighted_sum_0, weights_vec_0, hidden_vec);

                // Ładujemy 8-elementowe fragmenty acc[1][j] i L1_bias[j+16] oraz liczymy drugą część hidden
                __m256 acc_vec_1 = _mm256_loadu_ps(&acc[1][j]);
                __m256 bias_vec_1 = _mm256_loadu_ps(&L1_bias[j]);
                __m256 weighted_sum_1 = _mm256_add_ps(acc_vec_1, bias_vec_1);
                weighted_sum_1 = _mm256_max_ps(weighted_sum_1, zero); // ReLU

                // __m256 mask_1 = _mm256_cmp_ps(weighted_sum_1, zero, _CMP_GT_OS);
                // __m256 leaky_relu_1 = _mm256_blendv_ps(_mm256_mul_ps(weighted_sum_1, leaky_scale), weighted_sum_1, mask_1);

                __m256 weights_vec_1 = _mm256_loadu_ps(&L2_weights[i][j+16]);
                hidden_vec = _mm256_fmadd_ps(weighted_sum_1, weights_vec_1, hidden_vec);
            }

            // Sumujemy wartości w `hidden_vec` i zapisujemy do `hidden[i]`
            hidden[i] = _mm256_reduce_add_ps(hidden_vec);
        }
        // Druga pętla: wyliczanie `result`
        result = 0.0f;
        for (int i = 0; i < l2_size; i += 8) {
            __m256 hidden_vec = _mm256_loadu_ps(&hidden[i]);
            __m256 bias_vec = _mm256_loadu_ps(&L2_bias[i]);
            __m256 weighted_sum = _mm256_add_ps(hidden_vec, bias_vec);
            weighted_sum = _mm256_max_ps(weighted_sum, zero); // ReLU

            // __m256 mask = _mm256_cmp_ps(weighted_sum, zero, _CMP_GT_OS);
            // __m256 leaky_relu = _mm256_blendv_ps(_mm256_mul_ps(weighted_sum, leaky_scale), weighted_sum, mask);

            __m256 output_weights_vec = _mm256_loadu_ps(&output_weights[i]);
            __m256 result_vec = _mm256_mul_ps(weighted_sum, output_weights_vec);

            // Sumujemy wszystkie elementy w `result_vec` i dodajemy do `result`
            result += _mm256_reduce_add_ps(result_vec);
        }
    }else
    {

        __m256 zero = _mm256_setzero_ps(); // ReLU – dla wartości poniżej 0
        __m256 leaky_scale = _mm256_set1_ps(0.1f);
        // Pierwsza pętla: wyliczanie `hidden`
        for (int i = 0; i < l2_size; i++) {
            __m256 hidden_vec = _mm256_setzero_ps();

            for (int j = 0; j < l1_size / 2; j += 8) {
                // Ładujemy 8-elementowe fragmenty acc[0][j] i L1_bias[j] oraz liczymy pierwszą część hidden
                __m256 acc_vec_0 = _mm256_loadu_ps(&acc[1][j]);
                __m256 bias_vec_0 = _mm256_loadu_ps(&L1_bias[j]);
                __m256 weighted_sum_0 = _mm256_add_ps(acc_vec_0, bias_vec_0);

                // Leaky ReLU dla `weighted_sum_0`
                // __m256 mask_0 = _mm256_cmp_ps(weighted_sum_0, zero, _CMP_GT_OS);  // Maskowanie
                // __m256 leaky_relu_0 = _mm256_blendv_ps(_mm256_mul_ps(weighted_sum_0, leaky_scale), weighted_sum_0, mask_0);
                weighted_sum_0 = _mm256_max_ps(weighted_sum_0, zero); // ReLU

                __m256 weights_vec_0 = _mm256_loadu_ps(&L2_weights[i][j]);
                hidden_vec = _mm256_fmadd_ps(weighted_sum_0, weights_vec_0, hidden_vec);

                // Ładujemy 8-elementowe fragmenty acc[1][j] i L1_bias[j+16] oraz liczymy drugą część hidden
                __m256 acc_vec_1 = _mm256_loadu_ps(&acc[0][j]);
                __m256 bias_vec_1 = _mm256_loadu_ps(&L1_bias[j]);
                __m256 weighted_sum_1 = _mm256_add_ps(acc_vec_1, bias_vec_1);
                weighted_sum_1 = _mm256_max_ps(weighted_sum_1, zero); // ReLU

                // __m256 mask_1 = _mm256_cmp_ps(weighted_sum_1, zero, _CMP_GT_OS);
                // __m256 leaky_relu_1 = _mm256_blendv_ps(_mm256_mul_ps(weighted_sum_1, leaky_scale), weighted_sum_1, mask_1);

                __m256 weights_vec_1 = _mm256_loadu_ps(&L2_weights[i][j+16]);
                hidden_vec = _mm256_fmadd_ps(weighted_sum_1, weights_vec_1, hidden_vec);
            }

            // Sumujemy wartości w `hidden_vec` i zapisujemy do `hidden[i]`
            hidden[i] = _mm256_reduce_add_ps(hidden_vec);
        }
        // Druga pętla: wyliczanie `result`
        result = 0.0f;
        for (int i = 0; i < l2_size; i += 8) {
            __m256 hidden_vec = _mm256_loadu_ps(&hidden[i]);
            __m256 bias_vec = _mm256_loadu_ps(&L2_bias[i]);
            __m256 weighted_sum = _mm256_add_ps(hidden_vec, bias_vec);
            weighted_sum = _mm256_max_ps(weighted_sum, zero); // ReLU

            // __m256 mask = _mm256_cmp_ps(weighted_sum, zero, _CMP_GT_OS);
            // __m256 leaky_relu = _mm256_blendv_ps(_mm256_mul_ps(weighted_sum, leaky_scale), weighted_sum, mask);

            __m256 output_weights_vec = _mm256_loadu_ps(&output_weights[i]);
            __m256 result_vec = _mm256_mul_ps(weighted_sum, output_weights_vec);

            // Sumujemy wszystkie elementy w `result_vec` i dodajemy do `result`
            result += _mm256_reduce_add_ps(result_vec);
        }

    }
    return (result + output_bias)*100;
}
#endif
