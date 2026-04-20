/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 16:22:24 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/20 13:27:19 by abdnahal         ###   ########.fr       */
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
    return 1;
}

void debbug(t_coder *coder)
{
    pthread_mutex_lock(&coder->last_compile_mutex);
    coder->state = CODER_DEBUGGING;
    log_print(coder->sim, coder->id, "is debugging");
    pthread_mutex_unlock(&coder->last_compile_mutex); 
}

void refactor(t_coder *coder)
{
    pthread_mutex_lock(&coder->last_compile_mutex);
    coder->state = CODER_REFACTORING;
    log_print(coder->sim, coder->id, "is refactoring");
    pthread_mutex_unlock(&coder->last_compile_mutex);
}

void burnout(t_coder *coder)
{
    pthread_mutex_lock(&coder->last_compile_mutex);
    coder->state = CODER_BURNED_OUT;
    log_print(coder->sim, coder->id, "burned out");
    stop_simulation(coder->sim);
    pthread_mutex_unlock(&coder->last_compile_mutex);
}