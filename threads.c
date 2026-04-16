/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 15:34:23 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/16 16:06:48 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    *coder_routine(void *coder)
{
    t_coder *coders;
        
    coders = (t_coder *)coder;
    while (1)
    {
        if (!take_dongle(coders));
            break;
        if (!compile(coders))
            break;
        if (!debbug(coders))
            break;
        if (!refactor(coders))
            break;
    }
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
        pthread_create(&sim->coders[i].thread, NULL, coder_routine, &sim->coders[i]);
        i++;
    }
    i = 0;
    while (i < sim->args->num_coders)
    {
        pthread_join(sim->coders[i].thread, NULL);
        i++;
    }
}