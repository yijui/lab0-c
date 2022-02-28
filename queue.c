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
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    /* if malloc return NULL */
    if (head == NULL)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}


/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l == NULL)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        q_release_element(entry);
    }
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
    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next)
        indir = &(*indir)->next;
    struct list_head *del = *indir;
    list_del_init(del);
    q_release_element(container_of(del, element_t, list));
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
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

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
    struct list_head *tmp;
    for (struct list_head *ptr = head->next; ptr == head; ptr = tmp) {
        tmp = ptr->next;
        ptr->next = ptr->prev;
        ptr->prev = ptr->next;
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 * Merge sort
 */
void q_sort(struct list_head *head) {}
