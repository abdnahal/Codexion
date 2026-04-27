/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 08:36:43 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/27 09:04:25 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void ft_error(char *str)
{
    printf("%s\n", str);
    exit(1);
}

long    ft_atoi(const char *nptr, t_sim *sim)
{
        long i, (sign), (nbr);
        i = 0;
        sign = 1;
        nbr = 0;
        if (ft_strlen(nptr) > 10)
                free_all(sim), ("Out of range int value detected !");
        if (nptr[i] == '-' || nptr[i] == '+')
        {
                if (!(nptr[i + 1] >= '0' && nptr[i + 1] <= '9'))
                        free_all(sim), ft_error("Non Numeric value detected !");
                if (nptr[i] == '-')
                        sign *= -1;
                i++;
        }
        while (nptr[i] >= '0' && nptr[i] <= '9')
                nbr = nbr * 10 + nptr[i++] - '0';
        if (nptr[i] && !(nptr[i] >= '0' && nptr[i] <= '9'))
                free_all(sim), ft_error("Non Numeric value detected !");
        if (nbr > INT_MAX || nbr < INT_MIN)
                free_all(sim), ft_error("Out of range int value detected !");
        return (nbr * sign);
}

int     ft_strlen(const char *str)
{
        int     i;

        i = 0;
        while (str[i])
                i++;
        return (i);
}

void free_all(t_sim *sim)
{
        int i;

        if (!sim)
                return ;
        pthread_mutex_destroy(&sim->log_mutex);
        pthread_mutex_destroy(&sim->stop_mutex);
        i = 0;
        if (sim->dongles && sim->args)
        {
                while (i < sim->args->num_coders)
                {
                        pthread_mutex_destroy(&sim->dongles[i].mutex);
                        pthread_cond_destroy(&sim->dongles[i].cond);
                        pthread_mutex_destroy(&sim->coders[i].last_compile_mutex);
                        free(sim->dongles[i].waiters.entries);
                        i++;
                }
        }
        free(sim->args);
        free(sim->coders);
        free(sim->dongles);
        free(sim);
}

int sim_is_running(t_sim *sim)
{
    int running;

    pthread_mutex_lock(&sim->stop_mutex);
    running = sim->is_running;
    pthread_mutex_unlock(&sim->stop_mutex);
    return running;
}