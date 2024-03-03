#include "bsp-tree.h"

struct bsp_node* find_node(struct bsp_node *root, const uint16_t data)
{
    if (!root)
        return NULL;

    if (root->id == data)
        return root;

    if (data <= root->id)
        return find_node(root->child_left, data);
    else
        return find_node(root->child_right, data);
}

void add_child(struct bsp_node *root, struct bsp_node *child)
{
    if (child->id <= root->id) {
        if (!root->child_left)
            root->child_left = child;
        else
            add_child(root->child_left, child);
    }
    else {
        if (!root->child_right)
            root->child_right = child;
        else
            add_child(root->child_right, child);
    }
}

void print_pre_order_tree_walk(struct bsp_node *root) {
    if (!root)
        return;

    printf("%d\t", root->id);
    print_pre_order_tree_walk(root->child_left);
    print_pre_order_tree_walk(root->child_right);
}
