/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 15:13:15 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/20 16:24:48 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void log_print(t_sim *sim, int coder_id, char *msg)
{
    long now;

    pthread_mutex_lock(&sim->log_mutex);
    if (sim_is_running(sim))
    {
        now = get_time_ms() - sim->start_time;
        printf("%ld %d %s\n", now, coder_id, msg);
    }
    pthread_mutex_unlock(&sim->log_mutex);
}

int can_take_dongle(t_dongle *dongle, int coder_id, long now, long cooldown)
{
    t_waiter top;

    if (dongle->is_taken)
        return 0;
    if (cooldown > 0 && now - dongle->released_at < cooldown)
        return 0;
    if (!heap_peek(&dongle->waiters, &top))
        return 0;
    return (top.coder_id == coder_id);
}

int queue_waiter(t_dongle *dongle, t_coder *coder)
{
    t_waiter waiter;
    long request_ts;

    request_ts = get_time_ms();
    waiter.coder = coder;
    waiter.coder_id = coder->id;
    waiter.priority = scheduler_priority_for_waiter(coder->sim, coder, request_ts);
    return heap_push(&dongle->waiters, waiter);
}

int acquire_one_dongle(t_coder *coder, t_dongle *dongle)
{
    long now;
    struct timespec ts;

    pthread_mutex_lock(&dongle->mutex);
    if (!queue_waiter(dongle, coder))
    {
        pthread_mutex_unlock(&dongle->mutex);
        return 0;
    }
    while (sim_is_running(coder->sim))
    {
        now = get_time_ms();
        if (can_take_dongle(dongle, coder->id, now, coder->sim->args->dongle_cooldown))
        {
            heap_pop(&dongle->waiters, NULL);
            dongle->is_taken = 1;
            dongle->holder_id = coder->id;
            pthread_mutex_unlock(&dongle->mutex);
            log_print(coder->sim, coder->id, "has taken a dongle");
            return 1;
        }
        ts.tv_sec = 0;
        ts.tv_nsec = 5000000;
        pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
    }
    heap_remove_coder(&dongle->waiters, coder->id);
    pthread_mutex_unlock(&dongle->mutex);
    return 0;
}

void release_one_dongle(t_dongle *dongle, long cooldown)
{
    pthread_mutex_lock(&dongle->mutex);
    dongle->is_taken = 0;
    dongle->holder_id = 0;
    dongle->released_at = get_time_ms();
    pthread_cond_broadcast(&dongle->cond);
    pthread_mutex_unlock(&dongle->mutex);
    usleep(cooldown * 1000);
}

void taken_dongle(t_coder *coder)
{
    t_dongle *first;
    t_dongle *second;

    if (!coder || !coder->left_dongle)
        return ;
    if (!coder->right_dongle)
    {
        if (acquire_one_dongle(coder, coder->left_dongle))
            release_one_dongle(coder->left_dongle, coder->sim->args->dongle_cooldown);
        return ;
    }
    first = coder->left_dongle;
    second = coder->right_dongle;
    if (first->id > second->id)
    {
        first = coder->right_dongle;
        second = coder->left_dongle;
    }
    if (!acquire_one_dongle(coder, first))
        return ;
    if (!acquire_one_dongle(coder, second))
    {
        release_one_dongle(first, coder->sim->args->dongle_cooldown);
        // make_timespec(&time, coder->sim->args->dongle_cooldown + 10);
        // pthread_cond_timedwait(&second->cond, &second->mutex, &time);
        return ;
    }
    release_one_dongle(second, coder->sim->args->dongle_cooldown);
    release_one_dongle(first, coder->sim->args->dongle_cooldown);
}