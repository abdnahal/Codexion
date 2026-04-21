/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 15:34:23 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/21 15:47:45 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    *coder_routine(void *coder)
{
    t_coder *worker;
    long now;
    
    worker = (t_coder *)coder;
    while (sim_is_running(worker->sim))
    {
        now = get_time_ms();
        if (now >= worker->sim->args->time_to_burnout + worker->last_compile_start)
            burnout(worker);
        taken_dongle(worker);
        compile(worker);
        debug(worker);
        refactor(worker);
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
    pthread_create(&sim->monitor_thread, NULL, monitor_thread, sim);
    i = 0;
    while (i < sim->args->num_coders)
    {
        pthread_join(sim->coders[i].thread, NULL);
        i++;
    }
    pthread_join(sim->monitor_thread, NULL);
}

void *monitor_thread(void *sime)
{
    t_sim *sim;
    int i;
    int count;
    
    sim = (t_sim *)sime;
    while (sim_is_running(sim))
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