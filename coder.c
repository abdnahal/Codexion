/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 16:22:24 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/21 15:47:11 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int compile(t_coder *coder)
{    
    pthread_mutex_lock(&coder->last_compile_mutex);
    coder->state = CODER_COMPILING;
    coder->compile_count++;
    coder->last_compile_start = get_time_ms();
    log_print(coder->sim, coder->id, "is compiling");
    pthread_mutex_unlock(&coder->last_compile_mutex);
    usleep(coder->sim->args->time_to_compile * 1000);
    return 1;
}

void debug(t_coder *coder)
{
    pthread_mutex_lock(&coder->last_compile_mutex);
    coder->state = CODER_DEBUGGING;
    log_print(coder->sim, coder->id, "is debugging");
    pthread_mutex_unlock(&coder->last_compile_mutex);
    usleep(coder->sim->args->time_to_debug * 1000);
}

void refactor(t_coder *coder)
{
    pthread_mutex_lock(&coder->last_compile_mutex);
    coder->state = CODER_REFACTORING;
    log_print(coder->sim, coder->id, "is refactoring");
    pthread_mutex_unlock(&coder->last_compile_mutex);
    usleep(coder->sim->args->time_to_refactor * 1000);
}

void burnout(t_coder *coder)
{
    pthread_mutex_lock(&coder->last_compile_mutex);
    coder->state = CODER_BURNED_OUT;
    log_print(coder->sim, coder->id, "burned out");
    stop_simulation(coder->sim);
    pthread_mutex_unlock(&coder->last_compile_mutex);
}