#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "list.h"
#include "list_sort.h"
#include "merge_sort.h"
#include "queue.h"


typedef struct {
    struct list_head list;
    int val;
    int seq;
} elementInt_t;

#define SAMPLES 10000

static void create_sample(struct list_head *head,
                          elementInt_t *space,
                          int samples)
{
    printf("Creating sample\n");
    for (int i = 0; i < samples; i++) {
        elementInt_t *elem = space + i;
        elem->val = rand();
        list_add_tail(&elem->list, head);
    }
}

static void copy_list(struct list_head *from,
                      struct list_head *to,
                      elementInt_t *space)
{
    if (list_empty(from))
        return;

    elementInt_t *entry;
    list_for_each_entry (entry, from, list) {
        elementInt_t *copy = space++;
        copy->val = entry->val;
        copy->seq = entry->seq;
        list_add_tail(&copy->list, to);
    }
}

static int compare(void *priv,
                   const struct list_head *a,
                   const struct list_head *b)
{
    if (a == b)
        return 0;

    elementInt_t *elea =
        list_entry(a, elementInt_t, list);  // cppcheck-suppress nullPointer
    elementInt_t *eleb =
        list_entry(b, elementInt_t, list);  // cppcheck-suppress nullPointer

    if (priv)
        *((int *) priv) += 1;

    return elea->val - eleb->val;
}

bool check_list(struct list_head *head, int count)
{
    if (list_empty(head))
        return 0 == count;

    elementInt_t *entry, *safe;
    size_t ctr = 0;
    list_for_each_entry_safe (entry, safe, head, list) {
        ctr++;
    }
    int unstable = 0;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (entry->list.next != head) {
            if (entry->val > safe->val) {
                fprintf(stderr, "\nERROR: Wrong order\n");
                return false;
            }
            if (entry->val == safe->val && entry->seq > safe->seq)
                unstable++;
        }
    }
    if (unstable) {
        fprintf(stderr, "\nERROR: unstable %d\n", unstable);
        return false;
    }

    if (ctr != SAMPLES) {
        fprintf(stderr, "\nERROR: Inconsistent number of elements: %lu\n",
                (unsigned long) ctr);
        return false;
    }
    return true;
}

typedef void (*test_func_t)(void *priv,
                            struct list_head *head,
                            list_cmp_func_t cmp);

typedef struct {
    char *name;
    test_func_t impl;
} test_t;

int main(void)
{
    struct list_head sample_head, warmdata_head, testdata_head;
    int count;
    int nums = SAMPLES;

    /* Assume ASLR */
    srand((uintptr_t) &main);

    test_t tests[] = {
        //{.name = "timesort", .impl = timsort},
        {.name = "list_sort", .impl = list_sort},
        {.name = "list_mergeSort", .impl = list_mergeSort},
        {NULL, NULL},
    };
    test_t *test = tests;

    INIT_LIST_HEAD(&sample_head);

    elementInt_t *samples = malloc(sizeof(*samples) * SAMPLES);
    elementInt_t *warmdata = malloc(sizeof(*warmdata) * SAMPLES);
    elementInt_t *testdata = malloc(sizeof(*testdata) * SAMPLES);

    create_sample(&sample_head, samples, nums);

    while (test->impl) {
        clock_t time;
        double cpu_time_used;
        printf("==== Testing %s ====\n", test->name);
        /* Warm up */
        INIT_LIST_HEAD(&warmdata_head);
        INIT_LIST_HEAD(&testdata_head);
        copy_list(&sample_head, &testdata_head, testdata);
        copy_list(&sample_head, &warmdata_head, warmdata);
        time = clock();
        test->impl(&count, &warmdata_head, compare);
        cpu_time_used = clock() - time;
        printf("Testing time cost: %f cycles, (%f ms);\n", cpu_time_used,
               (double) 1000 * cpu_time_used / CLOCKS_PER_SEC);

        /* Test */
        count = 0;
        time = clock();
        test->impl(&count, &testdata_head, compare);
        cpu_time_used = clock() - time;
        printf("Sorting time cost: %f cycles, (%f ms);", cpu_time_used,
               (double) 1000 * cpu_time_used / CLOCKS_PER_SEC);
        printf("  Comparisons:    %d\n", count);
        printf("  List is %s\n",
               check_list(&testdata_head, nums) ? "sorted" : "not sorted");
        test++;
    }

    return 0;
}