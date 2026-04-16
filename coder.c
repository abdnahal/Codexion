/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 16:22:24 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/16 16:35:32 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int compile(t_coder *coder)
{
    int secs;
    
    pthread_mutex_lock(&coder->last_compile_mutex);
    secs = get_time_ms() - coder->last_compile_start;
    if (secs >= coder->sim->args->time_to_burnout)
    {
        bournout(coder);
        return 0;
    }
    coder->state = CODER_COMPILING;
    printf("%d %d is compiling\n", get_time_ms() - coder->sim->start_time, coder->id);
    pthread_mutex_unlock(&coder->last_compile_mutex);
}