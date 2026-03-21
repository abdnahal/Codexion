/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 08:15:56 by abdnahal          #+#    #+#             */
/*   Updated: 2026/03/21 08:35:39 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int parser(int ac, char **av)
{
    int coders;
    int burnout;
    int to_compile;
    int to_debug;
    int to_refactor;
    int compiles_req;
    int cooldown;
    int scheduler;

    int *vars[] = {&coders, &burnout, &to_compile,
        &to_debug, &to_refactor, &compiles_req,
    &cooldown, &scheduler};

    for (int i = 0; i < 7; i++)
    {
        *(vars[i]) = atoi(av[i+1]);
    }
    return 1;
}

int main(int ac, char **av)
{
    parser(ac, av);
}