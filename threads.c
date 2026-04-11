/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 15:34:23 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/10 16:42:40 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    *coder_routine(void *arg)
{
    t_args *args;
        
    args = (t_args *)arg;
    printf("thread launched\n");
    printf("Time to compile: %d\n", args->time_to_compile);
    return NULL;
}

// void take_dongle(t_sim *sim)
// {
//     if (sim->coders->right_dongle->is_taken)
// }


void launch_threads(t_sim *sim)
{
    int i;
    
    i = 0;
    while (i < sim->args->num_coders)
    {
        pthread_create(&sim->coders[i].thread, NULL, coder_routine, sim->args);
        i++;
    }
    i = 0;
    while (i < sim->args->num_coders)
    {
        pthread_join(sim->coders[i].thread, NULL);
        i++;
    }
}