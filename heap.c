/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 11:52:06 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/10 12:06:32 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_heap    *heap_init(t_sim *sim, int initial_capacity)
{
    t_heap *heap;
    int i;
    
    heap = malloc(sizeof(t_heap));
    if (!heap)
        return 0;
    heap->entries = malloc(sizeof(t_waiter) * sim->args->num_coders);
    i = 0;
    while (i < sim->args->num_coders)
    {
        heap->entries[i].coder = &sim->coders[i];
        heap->entries[i].coder_id = sim->coders[i].id;
        i++;
    }
    return heap;
}