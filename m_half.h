#ifndef M_HALF_HDR
#define M_HALF_HDR
#include <stdint.h>

#include "u_vector.h"

namespace m {

typedef uint16_t half;

half convertToHalf(float in);

// `in' must be 16-byte aligned pointer
u::vector<half> convertToHalf(const float *in, size_t length);

}

#endif
