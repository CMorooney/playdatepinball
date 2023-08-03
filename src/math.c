#include "math.h"

void get_normal_for_right_triangle(float alpha, float beta, float* vector_x, float* vector_y) {
  float c = sinf(alpha) * sinf(beta);
  *vector_x = cosf(beta) / c;
  *vector_y = sinf(beta) / c;
}
