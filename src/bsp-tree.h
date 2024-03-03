#ifndef BSP_TREE_H
#define BSP_TREE_H

#include <stdio.h>
#include <stdint.h>
#include "vector.h"

struct box {
    struct vector2i_t top_left;
    struct vector2i_t bottom_right;
};

struct bsp_node {
    /**
     * Unique identifier for the current binary space
     * partitioning node.
     */
    uint16_t id;

    /**
     * Starting coordinate of the splitter vector.
     */
    struct vector2i_t splitter_start;

    /**
     * Direction and magnitude of the splitter vector.
     */
    struct vector2i_t splitter_delta;

    /**
     * Bounding box to the left of the splitter, when
     * considering the direction following the splitter delta.
     */
    struct box left_box;

    /**
     * Bounding box to the right of the splitter, when
     * considering the direction following the splitter delta.
     */
    struct box right_box;

    /**
     * Left child of the current binary space partitioning node.
     */
    struct bsp_node *child_left;

    /**
     * Right child of the current binary space partitioning node.
     */
    struct bsp_node *child_right;
};

struct bsp_node* find_node(struct bsp_node *root, const uint16_t id);

void add_child(struct bsp_node *root, struct bsp_node *child);

void print_pre_order_tree_walk(struct bsp_node *root);

#endif /* BSP_TREE_ H */
