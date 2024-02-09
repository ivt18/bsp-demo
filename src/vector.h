#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

struct vector2f_t { float x, y; };
struct vector2i_t { int32_t x, y; };

float dot(const struct vector2f_t v, const struct vector2f_t u);

float len(const struct vector2f_t v);

struct vector2f_t norm(const struct vector2f_t v);

struct vector2i_t vector2f_to_vector2i(const struct vector2f_t v);

#endif // VECTOR_H
