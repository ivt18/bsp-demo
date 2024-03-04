#include "vector.h"
#include <math.h>

float dot(const vector2f_t v, const vector2f_t u)
{
    return (v.x * u.x) + (v.y * u.y);
}

float len(const vector2f_t v)
{
    return sqrtf(dot(v, v));
}

vector2f_t norm(const vector2f_t v)
{
    vector2f_t u;
    float l = len(v);
    u.x = v.x / l;
    u.y = v.y / l;
    return u;
}

vector2i_t vector2f_to_vector2i(const vector2f_t v)
{
    vector2i_t vi = { (int32_t) v.x, (int32_t) v.y };
    return vi;
}
