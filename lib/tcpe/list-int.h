#ifndef TCPE_LIST_INT_H
#define TCPE_LIST_INT_H

/*
 *  Intrusive list structure, similar to <linux/list.h>
 *  from the Linux kernel, or Boost.Instrusive from the
 *  Boost libraries.
 */

static inline void _estats_list_init(struct estats_list* ptr)
{
    ptr->next = ptr;
    ptr->prev = ptr;
}

static inline void _estats_list_add2(struct estats_list* new,
                                     struct estats_list* prev,
                                     struct estats_list* next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void _estats_list_add(struct estats_list* new,
                                    struct estats_list* head)
{
    _estats_list_add2(new, head, head->next);
}

static inline void _estats_list_add_tail(struct estats_list* new,
                                         struct estats_list* head)
{
    _estats_list_add2(new, head->prev, head);
}

static inline void _estats_list_del2(struct estats_list* prev,
                                     struct estats_list* next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void _estats_list_del(struct estats_list* entry)
{
    _estats_list_del2(entry->prev, entry->next);
    entry->prev = entry->next = 0;
}

static inline int _estats_list_empty(struct estats_list* head)
{
    return head->next == head;
}

#define TCPE_LIST_ENTRY(ptr, type, member) \
    ((type*)((char*)(ptr) - (unsigned long)(&((type*)0)->member)))
#define TCPE_LIST_FOREACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)
#define TCPE_LIST_FOREACH_PREV(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)
#define TCPE_LIST_FOREACH_SAFE(pos, tmp, head) \
    for (pos = (head)->next, tmp = pos->next; pos != (head); pos = tmp, tmp = pos->next)

#endif /* TCPE_LIST_INT_H */
