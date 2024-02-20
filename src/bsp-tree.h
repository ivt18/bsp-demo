#ifndef BSP_TREE_H
#define BSP_TREE_H

#include <stdio.h>
#include <stdint.h>

struct bsp_node {
    uint16_t data;
    struct bsp_node *child_left;
    struct bsp_node *child_right;
};

struct bsp_node* find_data(struct bsp_node *root, const uint16_t data);

void add_child(struct bsp_node *root, struct bsp_node *child);

void print_pre_order_tree_walk(struct bsp_node *root);

#endif /* BSP_TREE_ H */