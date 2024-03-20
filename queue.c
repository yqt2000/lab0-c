#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "merge_sort.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

// void printList(struct list_head *head)
// {
//     element_t *e, *s;
//     list_for_each_entry_safe (e, s, head, list)
//         printf("%s->", e->value);
//     printf("\n");
// }

// void printNodeChain(struct list_head *head)
// {
//     while (head) {
//         printf("%s->", list_entry(head, element_t, list)->value);
//         head = head->next;
//     }
//     printf("\n");
// }

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        free(head);
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    if (list_empty(head)) {
        free(head);
        return;
    }
    element_t *entry, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (!e) {
        free(e);
        return false;
    }
    if (!(e->value = strdup(s))) {
        q_release_element(e);
        return false;
    }
    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *e = malloc(sizeof(element_t));
    if (!e) {
        free(e);
        return false;
    }
    if (!(e->value = strdup(s))) {
        q_release_element(e);
        return false;
    }
    list_add_tail(&e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = list_first_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&e->list);
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = list_last_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&e->list);
    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head)
        return false;
    if (list_is_singular(head)) {
        q_release_element(list_first_entry(head, element_t, list));
        INIT_LIST_HEAD(head);
        return true;
    }
    struct list_head **slow, *fast;
    slow = &head;
    fast = head->next;
    while (fast != head) {
        slow = &(*slow)->next;
        fast = fast->next;
        if (fast != head) {
            fast = fast->next;
        }
    }
    element_t *e = list_entry(*slow, element_t, list);
    list_del(*slow);
    q_release_element(e);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head || list_empty(head))
        return false;

    element_t *c, *s = NULL;
    bool dup = false;
    list_for_each_entry_safe (c, s, head, list) {
        if (&s->list != head && strcmp(c->value, s->value) == 0) {
            list_del(&c->list);
            q_release_element(c);
            dup = true;
        } else if (dup) {
            list_del(&c->list);
            q_release_element(c);
            dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    for (struct list_head *c = head->next; c->next != head && c != head;
         c = c->next) {
        list_move(c, c->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *n, *s;
    list_for_each_safe (n, s, head) {
        list_move(n, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    int times = q_size(head) / k;
    struct list_head *cutLast = head;
    struct list_head cutList;

    for (int i = 0; i < times; i++) {
        INIT_LIST_HEAD(&cutList);
        struct list_head *cutHead;
        cutHead = cutLast;
        for (int j = 0; j < k; j++)
            cutLast = cutLast->next;

        list_cut_position(&cutList, cutHead, cutLast);
        q_reverse(&cutList);
        list_splice(&cutList, cutHead);
        cutLast = cutList.prev;
    }
}

int q_list_cmp(void *priv, const struct list_head *a, const struct list_head *b)
{
    if (a == b)
        return 0;
    element_t *elea =
        list_entry(a, element_t, list);  // cppcheck-suppress nullPointer
    element_t *eleb =
        list_entry(b, element_t, list);  // cppcheck-suppress nullPointer

    if (priv)
        *((int *) priv) += 1;

    return strcmp(elea->value, eleb->value);
};

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    list_mergeSort(NULL, head, q_list_cmp);
    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    element_t *e = list_first_entry(head, element_t, list);
    element_t *s = list_entry(e->list.next, element_t, list);

    for (; &s->list != head;
         e = s, s = list_entry(s->list.next, element_t, list)) {
        // check element e, if e>s delete it and move e to previous node
        while (&s->list != head && strcmp(e->value, s->value) > 0) {
            list_del(&e->list);
            q_release_element(e);
            if (s->list.prev != head) {
                e = list_entry(s->list.prev, element_t, list);
            } else {
                break;
            }
        }
    }
    return 1;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    element_t *e = list_first_entry(head, element_t, list);
    element_t *s = list_entry(e->list.next, element_t, list);

    for (; &s->list != head;
         e = s, s = list_entry(s->list.next, element_t, list)) {
        // check element e, if e<s delete it and move e to previous node
        while (&s->list != head && strcmp(e->value, s->value) < 0) {
            list_del(&e->list);
            q_release_element(e);
            if (s->list.prev != head) {
                e = list_entry(s->list.prev, element_t, list);
            } else {
                break;
            }
        }
    }
    return 1;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    if (!head || list_empty(head))
        return 0;
    queue_contex_t *q1 = list_entry(head->next, queue_contex_t, chain);
    queue_contex_t *qnode = NULL;
    if (list_is_singular(head))
        return q1->size;

    list_for_each_entry (qnode, head, chain) {
        if (qnode->id == q1->id)
            continue;
        list_splice_tail(qnode->q, q1->q);
        qnode->size = 0;
        INIT_LIST_HEAD(qnode->q);
        q1->size += qnode->size;
    }
    q_sort(q1->q, descend);
    return q1->size;
}
