#include <cstdint>
#include <cstddef>

#include <arm_neon.h>

extern "C" void add_ints(int * __restrict pa, int * __restrict pb, unsigned int n, int x)
{
    unsigned int i;

    for(i = 0; i < (n & ~3); i++)
        pa[i] = pb[i] + x;
}

extern "C" void myFunc()
{
    constexpr size_t matrixLength = 2;

    // const int32_t matrixA[matrixLength] = {1, 2, 3 ,4, 5};
    // const int32_t matrixB[matrixLength] = {6, 7, 8 ,9, 10};

    const int32_t matrixA[matrixLength] = {1, 2};
    const int32_t matrixB[matrixLength] = {6, 7};

    // int32x2_t arr;

    int32_t matrixRes[matrixLength] = {};

    for(size_t i = 0; i < matrixLength; ++i)
    {
        matrixRes[i] = matrixA[i] + matrixB[i];
    }
}
