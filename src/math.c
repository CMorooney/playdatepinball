#include "math.h"
#include <math.h>

Vector get_normal_for_right_triangle(float alpha, float beta) {
  Vector v1 = (Vector){ .x = cosf(alpha), .y = sinf(alpha) };
  Vector v2 = (Vector){ .x = cosf(beta), .y = sinf(beta) };
  Vector vH = add_vectors(v1, v2);
  return normalize_vector(invert_vector(vH));
}

Vector add_vectors(Vector v1, Vector v2) {
  return (Vector) { .x = v1.x + v2.x, .y = v1.y + v2.y};
}

Vector subtract_vectors(Vector v1, Vector v2) {
  return (Vector) { .x = v1.x - v2.x, .y = v1.y - v2.y };
}

Vector divide_vector(Vector v, float by) {
  return (Vector) { .x = v.x / by, .y = v.y / by };
}

Vector multiply_vector(Vector v, float by) {
  return (Vector) { .x = v.x * by, .y = v.y * by };
}

Vector invert_vector(Vector v) {
  return (Vector) { .x = -v.x, .y = -v.y };
}

Vector normalize_vector(Vector v) {
  return divide_vector(v, get_magnitude(v));
}

Vector mirror_vector(Vector v, Vector normalizedNormal) {
  float dot = dot_product(v, normalizedNormal);
  Vector reflected_normal = multiply_vector(normalizedNormal, 2 * dot);
  return subtract_vectors(v, reflected_normal);
}


float get_magnitude(Vector v) {
  return sqrtf(powf(v.x, 2) + powf(v.y, 2));
}

float dot_product(Vector v1, Vector v2) {
  float sum = 0.0f;
  sum += v1.x * v2.x;
  sum += v1.y * v2.y;
  return sum;
}

