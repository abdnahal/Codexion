#include "codexion.h"

long get_time_ms()
{
    long ms;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (long)tv.tv_sec;
}

void make_timespec(struct timespec *ts, long future_ms)
{
    ts->tv_sec = fu
}