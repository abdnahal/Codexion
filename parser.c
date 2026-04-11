/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 08:15:56 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/11 14:35:09 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int innit(long **vars, char **sc, t_sim *sim)
{
    sim->args = malloc(sizeof(t_args));
    sim->args->num_coders = *(vars[0]);
    sim->args->time_to_burnout = *(vars[1]);
    sim->args->time_to_compile = *(vars[2]);
    sim->args->time_to_debug = *(vars[3]);
    sim->args->time_to_refactor = *(vars[4]);
    sim->args->compiles_required = *(vars[5]);
    sim->args->dongle_cooldown = *(vars[6]);
    if (strcmp(*sc, "fifo"))
        sim->args->scheduler = FIFO;
    else if (strcmp(*sc, "edf"))
        sim->args->scheduler = EDF;
    else
        return 0;
    if (!innit_coders(sim) || !init_dongles(sim))
        return 0;
    bind_coder_dongles(sim);
    sim->is_running = 1;
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
        i++;
    }
    return 1;
}

int innit_coders(t_sim *sim)
{
    int i;
    
    sim->coders = malloc(sizeof(t_coder) * sim->args->num_coders);
    if (sim->coders == NULL)
        return 0;
    i = 0;
    while (i < sim->args->num_coders)
    {
        sim->coders[i].id = i+1;
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
    while (i < sim->args->num_coders - 1)
    {
        sim->coders[i].left_dongle = &sim->dongles[i];
        if (i+1 < sim->args->num_coders - 1)
            sim->coders[i].right_dongle = &sim->dongles[i+1];
        else
            sim->coders[i].right_dongle = &sim->dongles[0];
        i++;
    }
}

int main(int ac, char **av)
{
    long coders, (burnout), (to_compile), (to_debug), (to_refactor), (compiles_req);
    long cooldown;
    char *scheduler;
    int i;
    t_sim *sim;
    
    long *vars[] = {&coders, &burnout, &to_compile,
        &to_debug, &to_refactor, &compiles_req,
    &cooldown};
    char **sc = &scheduler;
    if (ac != 9)
        ft_error("Missing arguments");
    i = 0;
    while (i < 7)
    {
        *(vars[i]) = ft_atoi(av[i+1]);
        i++;
    }
    *sc = av[i];
    sim = malloc(sizeof(t_sim));
    if (!innit(vars, sc, sim))
        return 0;
    sim->start_time = get_time_ms();
    launch_threads(sim);
    free_all(sim);
    return 1;
}