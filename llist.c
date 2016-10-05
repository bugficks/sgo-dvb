#include "llist.h"

bool llist_add_batch(struct llist_node *new_first, struct llist_node *new_last,
                     struct llist_head *head)
{
        struct llist_node *first;

        do {
                new_last->next = first = ACCESS_ONCE(head->first);
        } while (cmpxchg(&head->first, first, new_first) != first);

        return !first;
}
