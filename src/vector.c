#include "vector.h"
#include <math.h>

float dot(const struct vector2f_t v, const struct vector2f_t u)
{
    return (v.x * u.x) + (v.y * u.y);
}

float len(const struct vector2f_t v)
{
    return sqrtf(dot(v, v));
}

struct vector2f_t norm(const struct vector2f_t v)
{
    struct vector2f_t u;
    float l = len(v);
    u.x = v.x / l;
    u.y = v.y / l;
    return u;
}
