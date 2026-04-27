/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 08:15:56 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/27 09:42:21 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int init(char **av, t_sim *sim)
{
    sim->start_time = get_time_ms();
    sim->is_running = 1;
    sim->args = malloc(sizeof(t_args));
    sim->args->num_coders = ft_atoi(av[1], sim);
    sim->args->time_to_burnout = ft_atoi(av[2], sim);
    sim->args->time_to_compile = ft_atoi(av[3], sim);
    sim->args->time_to_debug = ft_atoi(av[4], sim);
    sim->args->time_to_refactor = ft_atoi(av[5], sim);
    sim->args->compiles_required = ft_atoi(av[6], sim);
    sim->args->dongle_cooldown = ft_atoi(av[7], sim);
    if (strcmp(av[8], "fifo") == 0)
        sim->args->scheduler = FIFO;
    else if (strcmp(av[8], "edf") == 0)
        sim->args->scheduler = EDF;
    else
        free_all(sim), ft_error("Scheduler should be either fifo or edf");
    parser(sim);
    if (!init_coders(sim) || !init_dongles(sim))
    {
        free_all(sim);
        return 0;
    }
    bind_coder_dongles(sim);
    return 1;
}

int     init_dongles(t_sim *sim)
{
    int i;
    
    sim->dongles = malloc(sizeof(t_dongle) * sim->args->num_coders);
    if (!sim->dongles)
        return 0;
    i = 0;
    while (i < sim->args->num_coders)
    {
        sim->dongles[i].id = i+1;
        sim->dongles[i].is_taken = 0;
        sim->dongles[i].holder_id = 0;
        sim->dongles[i].released_at = 0;
        sim->dongles[i].waiters.entries = NULL;
        sim->dongles[i].waiters.size = 0;
        sim->dongles[i].waiters.capacity = 0;
        pthread_mutex_init(&sim->dongles[i].mutex, NULL);
        pthread_cond_init(&sim->dongles[i].cond, NULL);
        i++;
    }
    return 1;
}

int init_coders(t_sim *sim)
{
    int i;
    
    sim->coders = malloc(sizeof(t_coder) * sim->args->num_coders);
    if (sim->coders == NULL)
        return 0;
    i = 0;
    while (i < sim->args->num_coders)
    {
        sim->coders[i].state = CODER_WAITING;
        sim->coders[i].compile_count = 0;
        sim->coders[i].last_compile_start = sim->start_time;
        sim->coders[i].id = i+1;
        sim->coders[i].sim = sim;
        pthread_mutex_init(&sim->coders[i].last_compile_mutex, NULL);
        i++;
    }
    return 1;
}

void    bind_coder_dongles(t_sim *sim)
{
    int i;
    
    if (sim->args->num_coders == 1)
    {
        sim->coders[0].left_dongle = &sim->dongles[0];
        sim->coders[0].right_dongle = NULL;
        return ;
    }
    i = 0;
    while (i < sim->args->num_coders)
    {
        sim->coders[i].left_dongle = &sim->dongles[i];
        if (i == sim->args->num_coders - 1)
            sim->coders[i].right_dongle = &sim->dongles[0];
        else
            sim->coders[i].right_dongle = &sim->dongles[i+1];
        i++;
    }
}

int main(int ac, char **av)
{
    t_sim *sim;
    
    if (ac != 9)
        ft_error("Missing arguments");    
    sim = malloc(sizeof(t_sim));
    pthread_mutex_init(&sim->log_mutex, NULL);
    pthread_mutex_init(&sim->stop_mutex, NULL);
    if (!init(av, sim))
        return 1;
    launch_threads(sim);
    return 0;
}