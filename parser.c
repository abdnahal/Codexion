/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 08:15:56 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/08 16:18:39 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_args *innit(long **vars, char **sc)
{
    struct s_args *args = malloc(sizeof(t_args));
    if (args == NULL)
        return NULL;
    args->num_coders = *(vars[0]);
    args->time_to_burnout = *(vars[1]);
    args->time_to_compile = *(vars[2]);
    args->time_to_debug = *(vars[3]);
    args->time_to_refactor = *(vars[4]);
    args->compiles_required = *(vars[5]);
    args->dongle_cooldown = *(vars[6]);
    if (strcmp(*sc, "fifo"))
        args->scheduler = FIFO;
    else
        args->scheduler = EDF;
    return args;
}

t_coder *innit_coders(t_sim *sim)
{
    t_coder *coders;
    int i;
    
    sim->coders = malloc(sizeof(t_coder) * args->num_coders);
    if (coders == NULL)
        return NULL;
    while (i < args->num_coders)
    {
        coders[i].id = i+1;
    }
    return sim->coders;
}

int main(int ac, char **av)
{
    long coders;
    long burnout;
    long to_compile;
    long to_debug;
    long to_refactor;
    long compiles_req;
    long cooldown;
    char *scheduler;
    int i;
    t_sim *sim;
    
    long *vars[] = {&coders, &burnout, &to_compile,
        &to_debug, &to_refactor, &compiles_req,
    &cooldown};
    char *sc[] = {&scheduler};
    if (ac != 9)
        ft_error("Missing arguments");

    for (i = 0; i < 7; i++)
    {
        *(vars[i]) = atoi(av[i+1]);
    }
    *sc = av[i];
    sim = malloc(sizeof(t_sim));
    sim->args = innit(vars, sc);
    //printf("%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", args->num_coders, args->time_to_burnout, args->time_to_compile, args->time_to_debug, args->time_to_refactor, args->compiles_required, args->dongle_cooldown, args->scheduler);
    sim->coders = innit_coders(sim);
    launch_threads(sim);
    return 1;
}