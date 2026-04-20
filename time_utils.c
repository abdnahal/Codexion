/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/05 15:13:18 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/20 12:25:05 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long get_time_ms()
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
        return (0);
    return ((tv.tv_sec * 1000L) + (tv.tv_usec / 1000L));
}

void make_timespec(struct timespec *ts, long future_ms)
{
    if (!ts)
        return ;
    if (future_ms < 0)
        future_ms = 0;
    ts->tv_sec = future_ms / 1000L;
    ts->tv_nsec = (future_ms % 1000L) * 1000000L;
}