/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 08:36:43 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/18 14:23:51 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void ft_error(char *str)
{
    printf("%s\n", str);
    exit(1);
}

long    ft_atoi(const char *nptr)
{
        long i, (sign), (nbr);
        i = 0;
        sign = 1;
        nbr = 0;
        if (ft_strlen(nptr) > 10)
                ft_error("Out of range int value detected !");
        if (nptr[i] == '-' || nptr[i] == '+')
        {
                if (!(nptr[i + 1] >= '0' && nptr[i + 1] <= '9'))
                        ft_error("Non Numeric value detected !");
                if (nptr[i] == '-')
                        sign *= -1;
                i++;
        }
        while (nptr[i] >= '0' && nptr[i] <= '9')
                nbr = nbr * 10 + nptr[i++] - '0';
        if (nptr[i] && !(nptr[i] >= '0' && nptr[i] <= '9'))
                ft_error("Non Numeric value detected !");
        if (nbr > INT_MAX || nbr < INT_MIN)
                ft_error("Out of range int value detected !");
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
        i = 0;
        if (sim->dongles && sim->args)
        {
                while (i < sim->args->num_coders)
                {
                        pthread_mutex_destroy(&sim->dongles[i].mutex);
                        pthread_cond_destroy(&sim->dongles[i].cond);
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