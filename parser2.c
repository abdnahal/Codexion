/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 16:10:55 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/25 11:03:17 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void parser(t_sim *sim)
{
    if (sim->args->compiles_required <= 0)
        free_all(sim), ft_error("Compiles_required should be positive");
    if (sim->args->dongle_cooldown < 0)
        free_all(sim), ft_error("dongle_cooldown should be positive");
    if (sim->args->num_coders <= 0)
        free_all(sim), ft_error("Number of coders should be positive");
    if (sim->args->time_to_burnout < 0)
        free_all(sim), ft_error("Time to burnout should be positive");
    if (sim->args->time_to_compile < 0)
        free_all(sim), ft_error("Time to compile should be positive");
    if (sim->args->time_to_debug < 0)
        free_all(sim), ft_error("Time to debug should be positive");
    if (sim->args->time_to_refactor < 0)
        free_all(sim), ft_error("Time to refactor should be positive");
}