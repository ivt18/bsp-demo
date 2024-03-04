#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "vector.h"
#include "wad.h"

typedef vector2i_t Vertex;

typedef struct {
    uint16_t start_vertex, end_vertex,
             flags, line_type, sector_tag,
             left_side_def, right_side_def;
} Linedef;

typedef struct {
    uint8_t n_vertices, n_linedefs;
    Vertex vertices[255];
    Linedef linedefs[255];
} Map;

bool read_vertex(const WAD* wad, size_t offset, Vertex *vertex);

bool read_linedef(const WAD* wad, size_t offset, Linedef *linedef);

#endif // MAP_H