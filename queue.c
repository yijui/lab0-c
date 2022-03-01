#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
// struct list_head *q_new()
// {
//     struct list_head *head = malloc(sizeof(struct list_head));
//     /* if malloc return NULL */
//     if (head == NULL)
//         return NULL;
//     INIT_LIST_HEAD(head);
//     return head;
// }
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    // check malloc not return null (implies error or size is 0)
    if (head)
        INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
// void q_free(struct list_head *l)
// {
//     if (l == NULL)
//         return;
//     element_t *entry, *safe;
//     list_for_each_entry_safe (entry, safe, l, list) {
//         q_release_element(entry);
//     }
//     free(l);
// }
void q_free(struct list_head *l)
{
    if (!l)
        return;  // should not free NULL list


    // traverse and delete all entries and its value in the list
    element_t *entry = NULL, *next = NULL;
    list_for_each_entry_safe (entry, next, l, list) {
        free(entry->value);
        free(entry);
    }

    // free list
    free(l);
}
/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;
    // Create new element
    element_t *newE = malloc(sizeof(element_t));
    if (newE == NULL)
        return false;
    newE->value = malloc((strlen(s) + 1) * sizeof(char));
    if (newE->value == NULL) {
        free(newE);
        return false;
    }
    memset(newE->value, 0, (strlen(s) + 1));
    strncpy(newE->value, s, strlen(s));
    // Insert the new element to the head
    list_add(&newE->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;
    // Create new element
    element_t *newE = malloc(sizeof(element_t));
    if (newE == NULL)
        return false;
    newE->value = malloc((strlen(s) + 1) * sizeof(char));
    if (newE->value == NULL) {
        free(newE);
        return false;
    }
    memset(newE->value, 0, (strlen(s) + 1) * sizeof(char));
    strncpy(newE->value, s, strlen(s));
    // Insert the new element to the tail
    list_add_tail(&newE->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head))
        return NULL;
    element_t *elementR = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp != NULL) {
        memset(sp, 0, bufsize);
        strncpy(sp, elementR->value, bufsize - 1);
    }
    return elementR;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head))
        return NULL;
    element_t *elementR = list_last_entry(head, element_t, list);
    list_del(head->prev);
    if (sp != NULL) {
        memset(sp, 0, bufsize);
        strncpy(sp, elementR->value, bufsize - 1);
    }
    return elementR;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return 0;
    int size = 0;
    struct list_head *p;
    list_for_each (p, head)
        size++;
    return size;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * ⌊x⌋ denotes the largest integer less than or equal to x.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 * [leetcode 2095] delete-the-middle-node-of-a-linked-list
 */
bool q_delete_mid(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return false;
    struct list_head **indir = &(head->next);
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next)
        indir = &(*indir)->next;
    struct list_head *del = *indir;
    list_del_init(del);
    element_t *mid = list_entry(del, element_t, list);
    q_release_element(mid);
    return true;
}



/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 * [leetcode 82] remove-duplicates-from-sorted-list-ii/
 */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;


    LIST_HEAD(duplist);

    bool prev = false;
    element_t *ptr = list_entry(head->next, element_t, list), *next = ptr;

    for (bool same; next->list.next != head; ptr = next) {
        next = list_entry(ptr->list.next, element_t, list);
        same = !strcmp(ptr->value, next->value);
        if (same || prev)
            list_move(&ptr->list, &duplist);
        prev = same;
    }
    // don't forget last node
    if (prev)
        list_move(&ptr->list, &duplist);

    // delete each element in dup list
    list_for_each_entry_safe (ptr, next, &duplist, list) {
        free(ptr->value);
        free(ptr);
    }

    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 * [Leetcode 24] Swap Nodes in Pairs
 */
void q_swap(struct list_head *head)
{
    if (head == NULL || list_empty(head) || head->next->next == NULL)
        return;

    struct list_head *left = NULL;
    struct list_head *right = NULL;

    for (struct list_head *ptr = head->next; ptr != head && ptr->next != head;
         ptr = ptr->next) {
        left = ptr;
        right = ptr->next;

        left->prev->next = right;
        right->next->prev = left;
        left->next = right->next;
        right->prev = left->prev;
        left->prev = right;
        right->next = left;
    }
}
// void q_swap(struct list_head *head)
// {
//     if (!head || list_empty(head))
//         return;  // NULL, empty or single node no need to swap

//     struct list_head *left = head->next, *right = left->next;

//     do {
//         // swap
//         left->prev->next = right;
//         right->next->prev = left;
//         left->next = right->next;
//         right->prev = left->prev;
//         left->prev = right;
//         right->next = left;

//         // move to next
//         left = left->next;
//         right = left->next;

//     } while (left != head && right != head);
// }
/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return;
    struct list_head *tmp = NULL;
    for (struct list_head *ptr = head; tmp != head; ptr = tmp) {
        tmp = ptr->next;
        ptr->next = ptr->prev;
        ptr->prev = tmp;
    }
}


struct list_head *my_merge(struct list_head *a, struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;
    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (strcmp(list_entry(a, element_t, list)->value,
                   list_entry(b, element_t, list)->value) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

struct list_head *my_mergeSortList(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *sorted_tail = head;
    // scan list for not sorted node
    while (sorted_tail->next) {
        if (strcmp(list_entry(sorted_tail, element_t, list)->value,
                   list_entry(sorted_tail->next, element_t, list)->value) > 0) {
            break;
        }
        sorted_tail = sorted_tail->next;
    }
    if (!sorted_tail->next) {
        // all the list is sorted, just return
        return head;
    }
    if (sorted_tail != head) {
        struct list_head *not_sorted = sorted_tail->next;
        sorted_tail->next = NULL;
        return my_merge(head, my_mergeSortList(not_sorted));
    }
    // the first two node is not sorted,
    // use cycle detection to split list at middle
    struct list_head *slow = head;
    struct list_head *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;
    return my_merge(my_mergeSortList(head), my_mergeSortList(fast));
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (q_size(head) <= 1) {
        return;
    }
    struct list_head *list = head->next;
    head->prev->next = NULL;
    list = my_mergeSortList(list);
    head->next = list;

    // rebuild prev link
    struct list_head *i = head;
    while (i->next != NULL) {
        i->next->prev = i;
        i = i->next;
    }
    head->prev = i;
    i->next = head;
}