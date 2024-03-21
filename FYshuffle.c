#include "FYshuffle.h"
#include <stdio.h>
#include <stdlib.h>
#include "time.h"


// assume i and j both are node not the head.
void list_swap(struct list_head *i, struct list_head *j)
{
    if (i == j)
        return;
    struct list_head *prev_i = i->prev;
    if (j == prev_i) {
        list_move(j, i);
    } else {
        list_move(i, j);
        list_move(j, prev_i);
    }
}

// void tra(struct list_head *h){
//     element_t *e;
//     list_for_each_entry(e,h,list) {
//         printf("%s <=> ",e->value);
//     }
//     printf("\n");
// }

// To shuffle an array a of n elements (indices 0..n-1):
void FYshuffle(struct list_head *head)
{
    // for i from n−1 down to 1 do
    //     j ← random integer such that 0 ≤ j ≤ i
    //     exchange a[j] and a[i]
    if (!head || list_is_singular(head))
        return;
    int len = q_size(head);
    struct list_head *node_i = head->prev;
    for (int i = len - 1; i > 0; i--) {
        int r = rand() % (i + 1);
        struct list_head *node_j = head->next;
        for (int j = 0; j < r; j++)
            node_j = node_j->next;
        list_swap(node_i, node_j);
        node_i = node_j->prev;
    }
    return;
}
