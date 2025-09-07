#define INPUT_SIZE 768

#define L1_SIZE 64

#define L2_SIZE 1

#if defined(INT16) || defined(HCE)
    typedef int scoreType;
#else
    typedef float scoreType;
#endif

#define evalScale 1

#define oneLayer true