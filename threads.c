/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 15:34:23 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/08 16:12:57 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void *routine(void *sim)
{
    t_sim *sime;
    
    sime = (t_sim *)sim;
    printf("thread launched");
    return NULL;
}

void launch_threads(t_sim *sim)
{
    int i;
    
    i = 0;
    while (i < sim->args.num_coders)
    {
        pthread_create(&sim->coders[i].thread, NULL, routine, NULL);
    }
    i = 0;
    while (i < sim->args.num_coders)
    {
        pthread_join(sim->coders[i].thread, NULL);
    }
}