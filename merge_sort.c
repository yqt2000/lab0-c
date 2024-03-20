#include "merge_sort.h"
#include "queue.h"
#include "string.h"

// assume l1, l2 are ascending and return the asending merge result
struct list_head *mergeTwoSortedList(void *priv,
                                     struct list_head *l1,
                                     struct list_head *l2,
                                     list_cmp_func_t cmp)
{
    struct list_head head;
    INIT_LIST_HEAD(&head);
    while (l1 && l2) {
        element_t *e1 =
            list_entry(l1, element_t, list);  // cppcheck-suppress nullPointer
        element_t *e2 =
            list_entry(l2, element_t, list);  // cppcheck-suppress nullPointer
        if (cmp(priv, l1, l2) <= 0) {
            l1 = l1->next;
            list_add_tail(&e1->list, &head);
        } else {
            l2 = l2->next;
            list_add_tail(&e2->list, &head);
        }
    }
    if (l1) {
        head.prev->next = l1;
        l1->prev = head.prev;
    }
    if (l2) {
        head.prev->next = l2;
        l2->prev = head.prev;
    }
    return head.next;
}

// ascending
struct list_head *divideList(void *priv,
                             struct list_head *head,
                             list_cmp_func_t cmp)
{
    if (!head || !head->next)
        return head;
    struct list_head *slow = head;
    struct list_head *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    slow->prev->next = NULL;
    struct list_head *l1 = divideList(priv, head, cmp);
    struct list_head *l2 = divideList(priv, slow, cmp);

    // merge sorted l1 and sorted l2
    return mergeTwoSortedList(priv, l1, l2, cmp);
}

void list_mergeSort(void *priv, struct list_head *head, list_cmp_func_t cmp)
{
    if (list_empty(head) || list_is_singular(head))
        return;
    head->prev->next = NULL;

    head->next = divideList(priv, head->next, cmp);

    // reconnect the link(prev)
    struct list_head *cur;
    for (cur = head; cur->next; cur = cur->next)
        cur->next->prev = cur;
    cur->next = head;
    head->prev = cur;
}