# Codexion Walkthrough (No Ready-to-Use Code)

This document is a build roadmap for finishing the project with **only responsibilities, prototypes, and validation checkpoints**.

---

## 1) Suggested final file map

You can keep your current files and optionally add small focused files.

### Keep / complete
- `parser.c` → CLI parsing, numeric checks, scheduler checks, simulation bootstrap.
- `threads.c` → coder routine, monitor routine, thread launch/join.
- `time_utils.c` → millisecond time helpers and precise sleeps.
- `log.c` → synchronized logging only.
- `utils.c` → errors + cleanup helpers.
- `codexion.h` → all shared structs, constants, and prototypes.

### Optional split (recommended for clarity)
- `init.c` → all init functions (sim, coders, dongles, mutexes, cond vars).
- `dongle_queue.c` → waiter heap/priority queue operations.
- `scheduler.c` → FIFO/EDF priority calculation.
- `monitor.c` → burnout and completion checks.
- `cleanup.c` → destruction/free of all resources.

If your subject requires exactly the listed files, keep all functions in those files but preserve these responsibilities.

---

## 2) Core architecture required

- One shared `t_sim`.
- `num_coders` coder threads.
- One monitor thread.
- One dongle object per seat.
- Per-dongle waiting queue (min-heap) for FIFO/EDF arbitration.
- Global stop flag (`is_running`) protected by mutex.
- Global logging mutex to serialize output lines.

---

## 3) Function prototype checklist (headers only)

> Use these as **targets** in `codexion.h`. Keep names if you want, or rename consistently.

### parser/bootstrap layer
```text
int     parse_args(int ac, char **av, t_args *out);
int     parse_scheduler(const char *s, int *scheduler_out);
int     parse_positive_long(const char *s, long *out);
int     parse_positive_int(const char *s, int *out);
int     init_sim(t_sim *sim, const t_args *args);
int     init_coders(t_sim *sim);
int     init_dongles(t_sim *sim);
void    bind_coder_dongles(t_sim *sim);
```

### thread lifecycle
```text
int     launch_threads(t_sim *sim);
void    join_threads(t_sim *sim);
void    *coder_routine(void *arg);
void    *monitor_routine(void *arg);
```

### coder action flow
```text
int     coder_take_both_dongles(t_coder *coder);
void    coder_release_both_dongles(t_coder *coder);
int     take_one_dongle(t_coder *coder, t_dongle *dongle);
void    release_one_dongle(t_coder *coder, t_dongle *dongle);
void    coder_do_compile(t_coder *coder);
void    coder_do_debug(t_coder *coder);
void    coder_do_refactor(t_coder *coder);
```

### scheduler / wait-queue
```text
long    scheduler_priority_for_waiter(const t_sim *sim, const t_coder *coder, long request_ts);
int     heap_init(t_heap *heap, int initial_capacity);
void    heap_destroy(t_heap *heap);
int     heap_push(t_heap *heap, t_waiter w);
int     heap_pop(t_heap *heap, t_waiter *out);
int     heap_peek(const t_heap *heap, t_waiter *out);
int     heap_remove_coder(t_heap *heap, int coder_id);
```

### monitor/state helpers
```text
int     sim_is_running(t_sim *sim);
void    sim_stop(t_sim *sim);
int     all_coders_completed(t_sim *sim);
int     is_coder_burned_out(t_coder *coder);
long    coder_deadline_ms(t_coder *coder);
```

### time + sleep
```text
long    get_time_ms(void);
void    ms_sleep_interruptible(t_sim *sim, long duration_ms);
void    make_timespec_from_ms(struct timespec *ts, long abs_ms);
```

### logging
```text
void    log_event(t_sim *sim, int coder_id, const char *msg);
void    log_taken_dongle(t_coder *coder, int dongle_id);
void    log_compile(t_coder *coder);
void    log_debug(t_coder *coder);
void    log_refactor(t_coder *coder);
void    log_burnout(t_coder *coder);
```

### cleanup/errors
```text
void    ft_error(const char *msg);
void    destroy_sim(t_sim *sim);
void    destroy_coders(t_sim *sim);
void    destroy_dongles(t_sim *sim);
void    free_sim_allocations(t_sim *sim);
```

---

## 4) What each file should do

## `parser.c`
- Validate argument count and value domains.
- Parse scheduler (`fifo` or `edf`).
- Reject malformed numeric strings.
- Fill `t_args` only after full validation succeeds.
- Create and initialize `t_sim` startup path.

## `threads.c`
- Create all coder threads and monitor thread.
- Implement `coder_routine` main loop:
  - acquire dongles,
  - compile,
  - release,
  - debug,
  - refactor,
  - stop on global flag or completion.
- Join threads during shutdown.

## `time_utils.c`
- Return accurate ms timestamps.
- Provide interruptible sleep helper (checks stop flag periodically).
- Provide `timespec` conversion for timed waits.

## `log.c`
- Own all print output.
- Lock `log_mutex` around every line.
- Print relative timestamp (`now - sim->start_time`).
- Avoid logging regular actions after stop (except terminal burnout line if subject expects it).

## `utils.c`
- Fatal error helper.
- Cleanup helpers if you keep file count small.

## `init.c` (optional)
- Allocate/initialize coders and dongles.
- Init mutexes/cond vars.
- Attach left/right dongles per coder.

## `dongle_queue.c` (optional)
- Heap grow/shrink operations.
- Compare entries by priority.
- Remove waiting coder on cancellation/stop.

## `monitor.c` (optional)
- Poll every `MONITOR_SLEEP_MS`.
- Burnout detection from `last_compile_start`.
- Detect all-completed condition.
- Flip `is_running` and wake waiting threads.

## `cleanup.c` (optional)
- Destroy every mutex/cond var created.
- Free arrays in reverse init order.
- Guarantee no leak on early error path.

---

## 5) Shared-state locking contract (must be explicit)

- `sim->is_running` ↔ protected by `sim->stop_mutex`.
- `sim` output ↔ protected by `sim->log_mutex`.
- each `dongle` fields (`is_taken`, `holder_id`, `released_at`, `waiters`) ↔ protected by that dongle mutex.
- each coder timing fields (`last_compile_start`, optionally `compile_count`) ↔ protected by `coder->last_compile_mutex`.

Write this contract in comments and respect it everywhere.

---

## 6) FIFO vs EDF behavior target

- **FIFO:** waiter priority = request timestamp (`request_ts`). Smaller is earlier.
- **EDF:** waiter priority = coder burnout deadline:
  - `deadline = last_compile_start + time_to_burnout`
  - smaller deadline means higher urgency.

Heap is min-heap in both modes.

---

## 7) Incremental validation plan (what to test and when)

1. **Parsing tests**
   - wrong argc,
   - non-numeric values,
   - invalid scheduler string,
   - boundary values (`1`, large values).

2. **Init tests**
   - all mutex/cond init succeed,
   - one-coder edge case wiring,
   - left/right mapping correctness.

3. **Time/log tests**
   - timestamps monotonic,
   - no interleaved log lines under load.

4. **Thread tests**
   - all threads created/joined,
   - stop flag exits loops cleanly.

5. **Scheduler tests**
   - FIFO ordering deterministic by arrival,
   - EDF ordering deterministic by deadline,
   - cooldown enforced before re-acquire.

6. **Monitor tests**
   - burnout is detected exactly once,
   - all-completed stops gracefully.

7. **Race/deadlock tests**
   - high coder count stress run,
   - no deadlock,
   - no busy-spin.

8. **Memory/resource tests**
   - no leaks,
   - no destroyed-while-locked mutexes,
   - clean early-failure cleanup.

---

## 8) “Done” checklist

- [ ] Parses and validates all subject arguments.
- [ ] Correct ms timing (not seconds).
- [ ] Coder + monitor threading stable.
- [ ] Per-dongle queue with FIFO/EDF behavior.
- [ ] Burnout detection and global stop.
- [ ] Cooldown respected.
- [ ] Serialized logs.
- [ ] Full cleanup and no leaks.
- [ ] Handles `num_coders == 1` edge case per subject expectations.

---

## 9) Practical build order

1) parser + args validation  
2) init_sim/init_coders/init_dongles  
3) logging + time utilities  
4) basic thread launch/join  
5) dongle acquire/release (without scheduler)  
6) add heap queue + FIFO  
7) add EDF mode  
8) monitor thread burnout/all-completed  
9) full cleanup and stress validation

This order minimizes debugging complexity and makes failures easy to isolate.
