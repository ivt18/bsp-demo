#ifndef MAP_H
#define MAP_H

#include <stdint.h>

typedef struct linedef {
    uint8_t start, end;     // start and end vertices
} linedef;

struct {
    uint8_t n_vertices, n_linedefs;
    struct vector2i_t vertices[255];
    linedef linedefs[255];
} map;

#endif // MAP_H