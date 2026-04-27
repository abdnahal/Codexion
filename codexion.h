/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abdnahal <abdnahal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 08:04:16 by abdnahal          #+#    #+#             */
/*   Updated: 2026/04/27 09:05:15 by abdnahal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H


# include <stdio.h> 
# include <stdlib.h>     
# include <string.h>    
# include <unistd.h>    
# include <sys/time.h>   
# include <pthread.h>     
# include <time.h>        
# include <limits.h>



# define FIFO               0
# define EDF                1

# define CODER_COMPILING    0
# define CODER_DEBUGGING    1
# define CODER_REFACTORING  2
# define CODER_WAITING      3
# define CODER_BURNED_OUT   4

# define SUCCESS            0
# define FAILURE            1

# define MONITOR_SLEEP_MS   1    
# define HEAP_INIT_CAPACITY 8    



typedef struct s_sim        t_sim;
typedef struct s_coder      t_coder;
typedef struct s_dongle     t_dongle;
typedef struct s_waiter     t_waiter;
typedef struct s_heap       t_heap;
typedef struct s_args       t_args;

void ft_error(char *str);
void parser(t_sim *sim);
long get_time_ms();
void launch_threads(t_sim *sim);
int init_coders(t_sim* sim);
int is_in(t_heap *heap, int coder_id);
void *coder_routine(void *sim);
long    ft_atoi(const char *nptr, t_sim *sim);
int     ft_strlen(const char *str);
int init(char **av, t_sim *sim);
int     init_dongles(t_sim *sim);
void    bind_coder_dongles(t_sim *sim);
long    scheduler_priority_for_waiter(const t_sim *sim, const t_coder *coder, long request_ts);
t_heap  *heap_init(t_sim *sim, int initial_capacity);
void    heap_destroy(t_heap *heap);
int     heap_push(t_heap *heap, t_waiter w);
int     heap_pop(t_heap *heap, t_waiter *out);
int     heap_peek(const t_heap *heap, t_waiter *out);
int     heap_remove_coder(t_heap *heap, int coder_id);
void free_all(t_sim *sim);
int taken_dongle(t_coder *coder);
int sim_is_running(t_sim *sim);
void log_print(t_sim *sim, int coder_id, char *msg);
int compile(t_coder *coder);
void debug(t_coder *coder);
void refactor(t_coder *coder);
void burnout(t_coder *coder);
void *monitor_thread(void *sime);
void stop_simulation(t_sim *sim);
void make_timespec(struct timespec *ts, long future_ms);


typedef struct s_waiter
{
    int         coder_id;
    long        priority;
    t_coder     *coder;
}               t_waiter;


typedef struct s_heap
{
    t_waiter    *entries;
    int         size;
    int         capacity;
}               t_heap;


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



typedef struct s_args
{
    int     num_coders;
    long    time_to_burnout;
    long    time_to_compile;
    long    time_to_debug;
    long    time_to_refactor;
    int     compiles_required;
    long    dongle_cooldown;
    int     scheduler;              
}               t_args;


typedef struct s_sim
{
    t_args          *args;
    long            start_time;
    t_coder         *coders;
    t_dongle        *dongles;
    pthread_t       monitor_thread;
    int             is_running;
    pthread_mutex_t stop_mutex;
    pthread_mutex_t log_mutex;
}               t_sim;


#endif