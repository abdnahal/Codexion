/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 11:52:06 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/15 15:22:02 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void heap_swap(t_waiter *a, t_waiter *b)
{
    t_waiter tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

static int waiter_is_higher_priority(t_waiter a, t_waiter b)
{
    if (a.priority < b.priority)
        return 1;
    if (a.priority > b.priority)
        return 0;
    return (a.coder_id < b.coder_id);
}

static void sift_up(t_heap *heap, int index)
{
    int parent;

    while (index > 0)
    {
        parent = (index - 1) / 2;
        if (!waiter_is_higher_priority(heap->entries[index], heap->entries[parent]))
            break ;
        heap_swap(&heap->entries[index], &heap->entries[parent]);
        index = parent;
    }
}

static void sift_down(t_heap *heap, int index)
{
    int left;
    int right;
    int best;

    while (1)
    {
        left = (index * 2) + 1;
        right = left + 1;
        best = index;
        if (left < heap->size
            && waiter_is_higher_priority(heap->entries[left], heap->entries[best]))
            best = left;
        if (right < heap->size
            && waiter_is_higher_priority(heap->entries[right], heap->entries[best]))
            best = right;
        if (best == index)
            break ;
        heap_swap(&heap->entries[index], &heap->entries[best]);
        index = best;
    }
}

static int heap_grow(t_heap *heap)
{
    t_waiter *next;
    int next_capacity;

    if (heap->capacity <= 0)
        next_capacity = HEAP_INIT_CAPACITY;
    else
        next_capacity = heap->capacity * 2;
    next = malloc(sizeof(t_waiter) * next_capacity);
    if (!next)
        return 0;
    if (heap->entries && heap->size > 0)
        memcpy(next, heap->entries, sizeof(t_waiter) * heap->size);
    free(heap->entries);
    heap->entries = next;
    heap->capacity = next_capacity;
    return 1;
}

t_heap    *heap_init(t_sim *sim, int initial_capacity)
{
    t_heap *heap;
    int capacity;
    
    heap = malloc(sizeof(t_heap));
    if (!heap)
        return 0;
    capacity = initial_capacity;
    if (capacity < HEAP_INIT_CAPACITY)
        capacity = HEAP_INIT_CAPACITY;
    if (sim && sim->args && capacity < sim->args->num_coders)
        capacity = sim->args->num_coders;
    heap->entries = malloc(sizeof(t_waiter) * capacity);
    if (!heap->entries)
    {
        free(heap);
        return 0;
    }
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void    heap_destroy(t_heap *heap)
{
    if (!heap)
        return ;
    free(heap->entries);
    free(heap);
}

int heap_push(t_heap *heap, t_waiter w)
{
    if (!heap)
        return 0;
    if (heap->size >= heap->capacity && !heap_grow(heap))
        return 0;
    heap->entries[heap->size] = w;
    sift_up(heap, heap->size);
    heap->size++;
    return 1;
}

int heap_pop(t_heap *heap, t_waiter *out)
{
    if (!heap || heap->size == 0)
        return 0;
    if (out)
        *out = heap->entries[0];
    heap->size--;
    if (heap->size > 0)
    {
        heap->entries[0] = heap->entries[heap->size];
        sift_down(heap, 0);
    }
    return 1;
}

int heap_peek(const t_heap *heap, t_waiter *out)
{
    if (!heap || heap->size == 0)
        return 0;
    if (out)
        *out = heap->entries[0];
    return 1;
}

int heap_remove_coder(t_heap *heap, int coder_id)
{
    int i;

    if (!heap || heap->size == 0)
        return 0;
    i = 0;
    while (i < heap->size)
    {
        if (heap->entries[i].coder_id == coder_id)
        {
            heap->size--;
            if (i != heap->size)
            {
                heap->entries[i] = heap->entries[heap->size];
                sift_up(heap, i);
                sift_down(heap, i);
            }
            return 1;
        }
        i++;
    }
    return 0;
}

long    scheduler_priority_for_waiter(const t_sim *sim, const t_coder *coder, long request_ts)
{
    long deadline;

    if (!sim || !sim->args || sim->args->scheduler == FIFO)
        return request_ts;
    if (!coder)
        return request_ts;
    pthread_mutex_lock((pthread_mutex_t *)&coder->last_compile_mutex);
    deadline = coder->last_compile_start + sim->args->time_to_burnout;
    pthread_mutex_unlock((pthread_mutex_t *)&coder->last_compile_mutex);
    return deadline;
}