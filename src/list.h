/**
 * ty, command-line program to manage Teensy devices
 * http://github.com/Koromix/ty
 * Copyright (C) 2014 Niels Martignène
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TY_LIST_H
#define TY_LIST_H

#include "ty/common.h"

TY_C_BEGIN

typedef struct ty_list_head
{
    struct ty_list_head *prev;
    struct ty_list_head *next;
} ty_list_head;

#define TY_LIST_HEAD(head) \
    ty_list_head head = {&head, &head}

static inline void ty_list_init(ty_list_head *head)
{
    head->prev = head;
    head->next = head;
}

static inline void _ty_list_insert(ty_list_head *prev, ty_list_head *next, ty_list_head *head)
{
    prev->next = head;
    head->prev = prev;

    next->prev = head;
    head->next = next;
}

static inline void ty_list_append(ty_list_head *head, ty_list_head *n)
{
    _ty_list_insert(head->prev, head, n);
}

static inline void ty_list_prepend(ty_list_head *head, ty_list_head *n)
{
    _ty_list_insert(head, head->next, n);
}

static inline void ty_list_remove(ty_list_head *head)
{
    head->prev->next = head->next;
    head->next->prev = head->prev;

    ty_list_init(head);
}

static inline void ty_list_replace(ty_list_head *head, ty_list_head *n)
{
    n->next = head->next;
    n->next->prev = n;
    n->prev = head->prev;
    n->prev->next = n;

    ty_list_init(head);
}

static inline bool ty_list_empty(ty_list_head *head)
{
    return head->next == head;
}

static inline void _ty_list_slice(ty_list_head *prev, ty_list_head *next, ty_list_head *head)
{
    if (ty_list_empty(head))
        return;

    head->next->prev = prev;
    prev->next = head->next;

    head->prev->next = next;
    next->prev = head->prev;

    ty_list_init(head);
}

static inline void ty_list_append_list(ty_list_head *head, ty_list_head *list)
{
    _ty_list_slice(head->prev, head, list);
}

static inline void ty_list_prepend_list(ty_list_head *head, ty_list_head *list)
{
    _ty_list_slice(head, head->next, list);
}

#define ty_list_entry(head, type, member) \
    ((type *)((char *)(head) - (size_t)(&((type *)0)->member)))

#define ty_list_foreach(cur, head) \
    for (ty_list_head *cur = (head)->next, *TY_UNIQUE_ID(next) = cur->next; cur != (head); \
         cur = TY_UNIQUE_ID(next), TY_UNIQUE_ID(next) = cur->next)

TY_C_END

#endif