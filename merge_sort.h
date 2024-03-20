#include <stdbool.h>
#include "list.h"
#include "list_sort.h"
#include "queue.h"

// assume l1, l2 are ascending and return the asending merge result
struct list_head *mergeTwoSortedList(void *priv,
                                     struct list_head *,
                                     struct list_head *,
                                     list_cmp_func_t);

// ascending
struct list_head *divideList(void *priv, struct list_head *, list_cmp_func_t);

__attribute__((nonnull(2, 3))) void list_mergeSort(void *priv,
                                                   struct list_head *head,
                                                   list_cmp_func_t cmp);