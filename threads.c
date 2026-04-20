/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 15:34:23 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/20 13:18:12 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    *coder_routine(void *coder)
{
    t_coder *coders;
    long now;
    
    coders = (t_coder *)coder;
    while (sim_is_running(coders->sim))
    {
        now = get_time_ms();
        if (now >= coders->sim->args->time_to_burnout + coders->last_compile_start)
            burnout(coders);
        taken_dongle(coders);
        compile(coders);
        debbug(coders);
        refactor(coders);
    }
    return NULL;
}

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
    pthread_create(&sim->monitor_thread, NULL, monitor_thread, sim);
    pthread_join(sim->monitor_thread, NULL);
    while (i < sim->args->num_coders)
    {
        pthread_join(sim->coders[i].thread, NULL);
        i++;
    }
}

void *monitor_thread(void *sime)
{
    t_sim *sim;
    int i;
    int count;
    
    sim = (t_sim *)sime;
    while (1)
    {
        i = 0;
        count = 0;
        while (i < sim->args->num_coders)
        {
            if (sim->coders[i].compile_count == sim->args->compiles_required)
                count++;
            i++;
        }
        if (count == sim->args->num_coders)
        {
            stop_simulation(sim);
            break;
        }
        usleep(MONITOR_SLEEP_MS * 1000);
    }
    return NULL;
}

void stop_simulation(t_sim *sim)
{
    int i;

    i = 0;
    pthread_mutex_lock(&sim->stop_mutex);
    sim->is_running = 0;
    while (i < sim->args->num_coders)
    {
        pthread_mutex_lock(&sim->dongles[i].mutex);
        pthread_cond_broadcast(&sim->dongles[i].cond);
        pthread_mutex_unlock(&sim->dongles[i].mutex);
        i++;
    }
    pthread_mutex_unlock(&sim->stop_mutex);
}