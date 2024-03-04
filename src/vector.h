#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

typedef struct { float x, y; } vector2f_t;
typedef struct { int32_t x, y; } vector2i_t;

float dot(const vector2f_t v, const vector2f_t u);

float len(const vector2f_t v);

vector2f_t norm(const vector2f_t v);

vector2i_t vector2f_to_vector2i(const vector2f_t v);

#endif // VECTOR_H
