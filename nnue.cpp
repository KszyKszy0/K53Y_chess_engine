#include "nnue.h"
#include "enums.h"
#include <immintrin.h>
#include <iostream>


// Funkcja pomocnicza do sumowania elementów wektora AVX
inline float _mm256_reduce_add_ps(__m256 &vec) {
    __m256 hsum = _mm256_hadd_ps(vec, vec);
    hsum = _mm256_hadd_ps(hsum, hsum);
    __m128 hsum_low = _mm256_castps256_ps128(hsum);
    __m128 hsum_high = _mm256_extractf128_ps(hsum, 1);
    hsum_low = _mm_add_ps(hsum_low, hsum_high);
    return _mm_cvtss_f32(hsum_low);
}

#ifdef FLOAT
float firstLayer(Accumulator &accum, bool perspective)
{
    auto values = accum.values;

    float result = 0;
    float hidden[L2_SIZE] = {0};

    int offset0 = perspective ? 0 : L1_SIZE/2;
    int offset1 = perspective ? L1_SIZE/2 : 0;

    __m256 zero = _mm256_setzero_ps(); // ReLU – dla wartości poniżej 0

    // Pierwsza pętla: wyliczanie `hidden`
    for (int i = 0; i < L2_SIZE; i++) {
        __m256 hiddenVec = _mm256_setzero_ps();

        for (int j = 0; j < L1_SIZE / 2; j += 8) {

            __m256 acc0 = _mm256_load_ps(&values[j + offset0]);

            __m256 relu0 = _mm256_max_ps(acc0, zero); // ReLU

            __m256 weights0 = _mm256_load_ps(&L2_weights[i][j]);
            hiddenVec = _mm256_fmadd_ps(relu0, weights0, hiddenVec);


            __m256 acc1 = _mm256_load_ps(&values[j + offset1]);

            __m256 relu1 = _mm256_max_ps(acc1, zero); // ReLU

            __m256 weights1 = _mm256_load_ps(&L2_weights[i][j + (L1_SIZE/2)]);
            hiddenVec = _mm256_fmadd_ps(relu1, weights1, hiddenVec);
        }

        // Sumujemy wartości w `hidden_vec` i zapisujemy do `hidden[i]`
        hidden[i] = _mm256_reduce_add_ps(hiddenVec);
    }

    if(!oneLayer)
    {
        // Druga pętla: wyliczanie `result`
        result = 0.0f;
        for (int i = 0; i < L2_SIZE; i += 8) {
            __m256 hiddenVec = _mm256_loadu_ps(&hidden[i]);
            __m256 biasVec = _mm256_loadu_ps(&L2_bias[i]);
            __m256 sum = _mm256_add_ps(hiddenVec, biasVec);
            sum = _mm256_max_ps(sum, zero); // ReLU

            __m256 output_weights_vec = _mm256_loadu_ps(&output_weights[i]);
            __m256 result_vec = _mm256_mul_ps(sum, output_weights_vec);

            // Sumujemy wszystkie elementy w `result_vec` i dodajemy do `result`
            result += _mm256_reduce_add_ps(result_vec);
        }
    }else
    {
        result = hidden[0];
    }
    return (result + output_bias) * 100 * evalScale;
}
#endif

#ifdef FLOAT16
float firstLayer(Accumulator& accum, bool perspective)
{
    auto values = accum.values;

    float result = 0;
    float hidden[L2_SIZE] = {0};

    int offset0 = perspective ? 0 : L1_SIZE/2;
    int offset1 = perspective ? L1_SIZE/2 : 0;

    __m256 zero = _mm256_setzero_ps(); // ReLU – dla wartości poniżej 0
    
    // Pierwsza pętla: wyliczanie `hidden`
    for (int i = 0; i < L2_SIZE; i++) {
        __m256 hidden_vec = _mm256_setzero_ps();
        __m256 acc = _mm256_setzero_ps();

        for (int j = 0; j < L1_SIZE / 2; j += 16) {
            // Load and ReLU the input activations in FP32
            __m256 val0 = _mm256_max_ps(_mm256_load_ps(&values[j + offset0]), zero);
            __m256 val1 = _mm256_max_ps(_mm256_load_ps(&values[j + offset0 + 8]), zero);

            // Convert to BF16
            __m256bh val_bf16 = _mm256_cvtne2ps_pbh(val1, val0);  // val1 = high 8, val0 = low 8

            __m256bh w_bf16 = (__m256bh)_mm256_load_ps((float*)&L2_weights[i][j]);
    
            // Perform dot product in BF16, accumulate in FP32
            acc = _mm256_dpbf16_ps(acc, val_bf16, w_bf16);

            // Load and ReLU the input activations in FP32
            val0 = _mm256_max_ps(_mm256_load_ps(&values[j + offset1]), zero);
            val1 = _mm256_max_ps(_mm256_load_ps(&values[j + offset1 + 8]), zero);

            // Convert to BF16
            val_bf16 = _mm256_cvtne2ps_pbh(val1, val0);  // val1 = high 8, val0 = low 8

            w_bf16 = (__m256bh)_mm256_load_ps((float*)&L2_weights[i][j + (L1_SIZE/2)]);


            // Perform dot product in BF16, accumulate in FP32
            acc = _mm256_dpbf16_ps(acc, val_bf16, w_bf16);
        }

        // Sumujemy wartości w `hidden_vec` i zapisujemy do `hidden[i]`
        hidden[i] = _mm256_reduce_add_ps(acc);
    }

    if(!oneLayer)
    {
        // Druga pętla: wyliczanie `result`
        result = 0.0f;
        for (int i = 0; i < L2_SIZE; i += 8) {
            __m256 hidden_vec = _mm256_loadu_ps(&hidden[i]);
            __m256 bias_vec = _mm256_loadu_ps(&L2_bias[i]);
            __m256 weighted_sum = _mm256_add_ps(hidden_vec, bias_vec);
            weighted_sum = _mm256_max_ps(weighted_sum, zero); // ReLU

            __m256 output_weights_vec = _mm256_loadu_ps(&output_weights[i]);
            __m256 result_vec = _mm256_mul_ps(weighted_sum, output_weights_vec);

            // Sumujemy wszystkie elementy w `result_vec` i dodajemy do `result`
            result += _mm256_reduce_add_ps(result_vec);
        }
    }
    else
    {
        result = hidden[0];
    }

    return (result + output_bias) * 100 * evalScale;
}
#endif

#ifdef INT16
float firstLayer(Accumulator& accum, bool perspective)
{
    auto values = accum.values;

    // 1. Deklarujemy tablicę ukrytą w int32
    int32_t hidden[L2_SIZE] = {0};

    // Dzielimy L1_SIZE na dwie połowy w zależności od perspektywy
    int offset0 = perspective ? 0 : L1_SIZE/2;
    int offset1 = perspective ? L1_SIZE/2 : 0;

    // 2. Pierwsza warstwa: liczymy hidden
    for (int i = 0; i < L2_SIZE; ++i) {
        int32_t acc_sum = 0;

        for (int j = 0; j < L1_SIZE/2; ++j) {
            // Pobieramy values i bias, rzutujemy do int32
            int32_t a0 = static_cast<int32_t>(values[j + offset0]);
            int32_t b0 = static_cast<int32_t>(L1_bias[j]);
            int32_t x0 = a0 + b0;
            // ReLU
            x0 = std::max(0, x0);
            // waga
            int32_t w0 = static_cast<int32_t>(L2_weights[i][j]);
            acc_sum += x0 * w0;

            // Druga część z drugiej połowy values
            int32_t a1 = static_cast<int32_t>(values[j + offset1]);
            int32_t b1 = static_cast<int32_t>(L1_bias[j]);
            int32_t x1 = a1 + b1;
            x1 = std::max(0, x1);
            int32_t w1 = static_cast<int32_t>(L2_weights[i][j + L1_SIZE/2]);
            acc_sum += x1 * w1;
        }
        hidden[i] = acc_sum;
    }

    int32_t result = 0;
    if(!oneLayer)
    {
        // 3. Druga warstwa: liczymy wynik
        for (int i = 0; i < L2_SIZE; ++i) {
            int32_t h = hidden[i] + static_cast<int32_t>(L2_bias[i]);
            h = std::max(0, h);
            int32_t w = static_cast<int32_t>(output_weights[i]);
            result += h * w;
        }
    }
    else
    {
        result = hidden[0];
    }
    

    // 4. Dodajemy bias wyjściowy i skalujemy
    int32_t final_acc = result_acc + static_cast<int32_t>(output_bias);
    return static_cast<float>(final_acc) * evalScale / 1000.0f;
}
#endif