/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 15:34:23 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/25 11:16:14 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void    *coder_routine(void *coder)
{
    t_coder *worker;
    
    worker = (t_coder *)coder;
    while (sim_is_running(worker->sim))
    {
        if (taken_dongle(worker))
        {
            compile(worker);
            if (sim_is_running(worker->sim))
                debug(worker);
            if (sim_is_running(worker->sim))
                refactor(worker);
        }
    }
    return NULL;
}

void launch_threads(t_sim *sim)
{
    int i;
    
    i = 0;
    if (pthread_create(&sim->monitor_thread, NULL, monitor_thread, sim))
        free_all(sim), ft_error("Monitor thread creation failed!");
    while (i < sim->args->num_coders)
    {
        if (pthread_create(&sim->coders[i].thread, NULL, coder_routine, &sim->coders[i]))
            free_all(sim), ft_error("Thread creation (pthread_create) failed!");
        i++;
    }
    i = 0;
    while (i < sim->args->num_coders)
    {
        if (pthread_join(sim->coders[i].thread, NULL))
            free_all(sim), ft_error("Pthread_join failed!");
        i++;
    }
    if (pthread_join(sim->monitor_thread, NULL))
        free_all(sim), ft_error("Monitor thread join failed!");
    free_all(sim);
}

void *monitor_thread(void *sime)
{
    t_sim *sim;
    int i, (count), (compile_count);
    long last_compile, (now);
    
    sim = (t_sim *)sime;
    while (sim_is_running(sim))
    {
        i = 0;
        count = 0;
        now = get_time_ms();
        while (i < sim->args->num_coders)
        {
            pthread_mutex_lock(&sim->coders[i].last_compile_mutex);
            compile_count = sim->coders[i].compile_count;
            last_compile = sim->coders[i].last_compile_start;
            pthread_mutex_unlock(&sim->coders[i].last_compile_mutex);
            if (compile_count >= sim->args->compiles_required)
                count++;
            if (last_compile + sim->args->time_to_burnout <= now)
            {
                burnout(&sim->coders[i]);
                break ;
            }
            i++;
        }
        if (!sim_is_running(sim))
            break ;
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
    if (!sim->is_running)
    {
        pthread_mutex_unlock(&sim->stop_mutex);
        return ;
    }
    sim->is_running = 0;
    pthread_mutex_unlock(&sim->stop_mutex);
    while (i < sim->args->num_coders)
    {
        pthread_mutex_lock(&sim->dongles[i].mutex);
        pthread_cond_broadcast(&sim->dongles[i].cond);
        pthread_mutex_unlock(&sim->dongles[i].mutex);
        i++;
    }
}