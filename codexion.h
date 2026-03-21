/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 08:04:16 by abdnahal          #+#    #+#             */
/*   Updated: 2026/03/21 08:04:42 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

/* ========================================================================= */
/*                              INCLUDES                                     */
/* ========================================================================= */

# include <stdio.h>       /* printf, fprintf                                 */
# include <stdlib.h>      /* malloc, free, atoi, exit                        */
# include <string.h>      /* memset, strlen, strcmp                          */
# include <unistd.h>      /* write, usleep                                   */
# include <sys/time.h>    /* gettimeofday, struct timeval                    */
# include <pthread.h>     /* all pthread_* functions                         */
# include <time.h>        /* struct timespec (for pthread_cond_timedwait)    */

/* ========================================================================= */
/*                              CONSTANTS                                    */
/* ========================================================================= */

# define FIFO               0
# define EDF                1

# define CODER_COMPILING    0
# define CODER_DEBUGGING    1
# define CODER_REFACTORING  2
# define CODER_WAITING      3
# define CODER_BURNED_OUT   4

# define SUCCESS            0
# define FAILURE            1

# define MONITOR_SLEEP_MS   1    /* how often monitor checks burnout (ms)   */
# define HEAP_INIT_CAPACITY 8    /* initial heap capacity (grows as needed)  */

/* ========================================================================= */
/*                           FORWARD DECLARATIONS                            */
/* ========================================================================= */

typedef struct s_sim        t_sim;
typedef struct s_coder      t_coder;
typedef struct s_dongle     t_dongle;
typedef struct s_waiter     t_waiter;
typedef struct s_heap       t_heap;
typedef struct s_args       t_args;

/* ========================================================================= */
/*                           HEAP / PRIORITY QUEUE                          */
/* ========================================================================= */

/*
** t_waiter: one entry in a dongle's waiting queue.
**
** coder_id   - which coder is waiting (1-indexed, matches log output)
** priority   - the value used for heap ordering:
**              FIFO → timestamp of request arrival (smaller = earlier)
**              EDF  → burnout deadline (smaller = more urgent)
** coder      - direct pointer to the coder struct for fast access
*/
typedef struct s_waiter
{
    int         coder_id;
    long        priority;
    t_coder     *coder;
}               t_waiter;

/*
** t_heap: a min-heap of t_waiter entries.
**
** entries    - the array storing heap nodes
** size       - current number of elements in the heap
** capacity   - current allocated size of the entries array
**              (doubles when full, so we rarely need to realloc)
*/
typedef struct s_heap
{
    t_waiter    *entries;
    int         size;
    int         capacity;
}               t_heap;

/* ========================================================================= */
/*                              DONGLE                                       */
/* ========================================================================= */

/*
** t_dongle: represents one USB dongle on the table.
**
** id          - dongle index (0-indexed internally)
** mutex       - protects ALL fields of this struct.
**               Any thread reading or writing below must hold this lock.
** cond        - condition variable. Threads call pthread_cond_wait here
**               while waiting for the dongle to become available.
**               pthread_cond_broadcast is called on release and shutdown.
** is_taken    - 1 if a coder currently holds this dongle, 0 if free.
** holder_id   - which coder currently holds it (for debugging/logging).
**               Only valid when is_taken == 1.
** released_at - timestamp (ms) of the last release. Used to enforce
**               dongle_cooldown. Initialized to 0 (no cooldown at start).
** waiters     - priority queue of coders waiting for this dongle.
**               Ordered by FIFO arrival time or EDF deadline depending
**               on the scheduler. Access always under mutex.
*/
typedef struct s_dongle
{
    int             id;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int             is_taken;
    int             holder_id;
    long            released_at;
    t_heap          waiters;
}               t_dongle;

/* ========================================================================= */
/*                               CODER                                       */
/* ========================================================================= */

/*
** t_coder: represents one coder (one pthread).
**
** id                  - coder number, 1 to number_of_coders (as in logs)
** thread              - the pthread handle, used for pthread_join in cleanup
** state               - current activity: COMPILING, DEBUGGING, etc.
**                       Written by coder thread, read by monitor thread.
**                       Protect with state_mutex if you read/write from
**                       multiple threads simultaneously.
** compile_count       - how many times this coder has successfully compiled.
**                       Written only by the coder's own thread, read by
**                       the monitor. Use an int and rely on monitor polling
**                       with its own timing, or protect with a mutex.
** last_compile_start  - timestamp (ms) of when this coder last STARTED
**                       compiling (or simulation start if not yet compiled).
**                       CRITICAL for burnout detection and EDF priority.
**                       Written by coder thread, read by monitor.
**                       Protect with last_compile_mutex.
** last_compile_mutex  - protects last_compile_start and compile_count
**                       from concurrent read/write between coder and monitor.
** left_dongle         - pointer to the dongle on this coder's left side.
**                       Coder N's left dongle is dongles[N-1] (0-indexed).
** right_dongle        - pointer to the dongle on this coder's right side.
**                       Coder N's right dongle is dongles[N] (0-indexed).
** sim                 - back-pointer to the shared simulation state.
**                       Gives access to all global params and stop flag.
*/
typedef struct s_coder
{
    int             id;
    pthread_t       thread;
    int             state;
    int             compile_count;
    long            last_compile_start;
    pthread_mutex_t last_compile_mutex;
    t_dongle        *left_dongle;
    t_dongle        *right_dongle;
    t_sim           *sim;
}               t_coder;

/* ========================================================================= */
/*                            PARSED ARGUMENTS                               */
/* ========================================================================= */

/*
** t_args: holds the validated, converted command-line arguments.
** Populated by validate.c before any initialization begins.
** All time values are stored in milliseconds as long integers.
*/
typedef struct s_args
{
    int     num_coders;
    long    time_to_burnout;
    long    time_to_compile;
    long    time_to_debug;
    long    time_to_refactor;
    int     compiles_required;
    long    dongle_cooldown;
    int     scheduler;              /* FIFO or EDF constant */
}               t_args;

/* ========================================================================= */
/*                          SIMULATION STATE                                 */
/* ========================================================================= */

/*
** t_sim: the master struct passed to every thread.
** One instance exists for the entire program lifetime.
**
** args             - a copy of the parsed arguments. Read-only after init.
** start_time       - simulation start timestamp in ms. Used to compute
**                    relative timestamps for all log output.
** coders           - heap-allocated array of t_coder, length = num_coders.
** dongles          - heap-allocated array of t_dongle, length = num_coders.
**                    (special case: 1 coder → 1 dongle)
** monitor_thread   - pthread handle for the monitor. Joined in cleanup.
**
** is_running       - shared stop flag. Set to 0 by monitor on burnout or
**                    when all coders reach compiles_required. Coder threads
**                    check this in their main loop and after waking from
**                    cond waits. MUST be read/written under stop_mutex.
** stop_mutex       - protects is_running.
**
** log_mutex        - protects all output. Every printf/write in the
**                    entire program must lock this first to prevent
**                    interleaved log lines.
*/
typedef struct s_sim
{
    t_args          args;
    long            start_time;
    t_coder         *coders;
    t_dongle        *dongles;
    pthread_t       monitor_thread;
    int             is_running;
    pthread_mutex_t stop_mutex;
    pthread_mutex_t log_mutex;
}               t_sim;

#endif