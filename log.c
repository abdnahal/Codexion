/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 15:13:15 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/13 12:01:33 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void log_print(t_sim *sim, int coder_id, char *msg)
{
    int time;
    pthread_mutex_lock(&sim->log_mutex);
    if (sim->is_running)
    {
        time = get_time_ms() - sim->start_time;
        printf("%d %d %s\n", time, coder_id, msg);
    }
    pthread_mutex_unlock(&sim->log_mutex);
}

void log_taken_dongle(t_coder *coder)
{
    pthread_mutex_lock(&coder->last_compile_mutex);
    if (!coder->right_dongle->is_taken)
    {
        coder->right_dongle->holder_id = coder->id;
        coder->right_dongle->is_taken = 1;
    }
    else if (!coder->left_dongle->is_taken)
    {
        coder->left_dongle->holder_id = coder->id;
        coder->left_dongle->is_taken = 1;
    }
    printf("%d %d has taken a dongle", coder->last_compile_start, coder->id);
    pthread_mutex_unlock(&coder->last_compile_mutex);
}