# Codexion Step-by-Step To-Do List

Use this as an execution checklist. Complete in order.

---

## Current position

The parser/allocation bug is fixed. Your next work should be focused and local:

1. make logging safe and complete,
2. make the thread routine actually use a coder context,
3. finish the heap as a real priority queue,
4. wire dongle acquire/release around that queue,
5. then add monitor stop logic and cleanup.

If you want the shortest path, start at Step 5 and do not move forward until logging is stable.

---

## Do next now

### Task A: finish `log.c`
- [ ] Make one generic logger that locks `log_mutex`, prints one line, and unlocks.
- [ ] Keep logging relative to `start_time`.
- [ ] Remove or complete unfinished helpers that lock a mutex and do not unlock it.

### Task B: fix `threads.c` around a coder context
- [ ] Each thread should receive the coder it controls, not only the shared args.
- [ ] The routine should read coder fields from that context.
- [ ] Replace placeholder prints with logger calls.

### Task C: turn `heap.c` into a real queue
- [ ] Initialize an empty heap.
- [ ] Track `size` and `capacity`.
- [ ] Add push, pop, peek, and remove-by-coder behavior.

### Task D: use the heap in dongle acquisition
- [ ] Push a waiter when a coder wants a dongle.
- [ ] Grant the dongle only when the waiter is highest priority and cooldown is satisfied.
- [ ] Broadcast on release.

### Task E: add monitor stop behavior
- [ ] Poll burnout.
- [ ] Stop when everyone has compiled enough.
- [ ] Wake blocked threads on shutdown.

---

## Step 1 — Lock your public API first

- [ ] In `codexion.h`, make all prototypes match implementations exactly.
- [ ] Keep one naming style (`init_*` or `innit_*`) and remove duplicates.
- [ ] Ensure return types are consistent (`int` status vs pointer return).
- [ ] Add missing prototypes used across files (`init_dongles`, `bind_coder_dongles`, logger helpers, etc.).

**Exit condition:** project compiles headers cleanly with no conflicting declarations.

---

## Step 2 — Fix argument parsing and ownership

- [ ] In `parser.c`, split parsing from initialization.
- [ ] Parse CLI values into a local `t_args` (fully validated) before allocation.
- [ ] Validate scheduler string strictly (`fifo` / `edf`).
- [ ] Validate numeric domains (positive values, range limits).
- [ ] Allocate `t_sim` and `t_args` safely, then copy parsed values.
- [ ] Ensure `main` keeps `t_sim *sim` and passes it to launcher.

**Exit condition:** invalid input always fails cleanly; valid input builds a usable `t_sim`.

**Status:** likely done or nearly done if the allocation bug is fixed.

---

## Step 3 — Complete initialization path

- [ ] In `parser.c` (or split init file), finish `init_coders`.
- [ ] Finish `init_dongles` including per-dongle defaults.
- [ ] Initialize required mutexes/conds (`stop_mutex`, `log_mutex`, dongle mutex/cond, coder timing mutex).
- [ ] Finish `bind_coder_dongles` loop and include `i++` progression.
- [ ] Handle `num_coders == 1` edge case intentionally.

**Exit condition:** full simulation objects are initialized without UB or leaks on failure.

**Status:** likely done or nearly done if the invalid write / invalid read errors are gone.

---

## Step 4 — Finalize time utilities

- [x] `get_time_ms` returns milliseconds.
- [x] `make_timespec` converts ms to `timespec`.
- [ ] Add an interruptible sleep helper for action timing.
- [ ] Use consistent absolute/relative time semantics everywhere.

**Exit condition:** all timing logic uses ms consistently.

**Status:** already started; keep it correct before moving on to thread timing.

---

## Step 5 — Build reliable logging

- [ ] In `log.c`, keep one central `log_event` function with `log_mutex`.
- [ ] Print relative timestamp from `sim->start_time`.
- [ ] Add wrapper events (taken dongle, compiling, debugging, refactoring, burnout).
- [ ] Ensure no partial/unfinished logger functions remain.
- [ ] Make sure any helper that locks a mutex also unlocks it on every path.

**Exit condition:** no interleaved log lines under thread contention.

**This is the first thing to finish next.**

---

## Step 6 — Repair thread creation and routine skeleton

- [ ] Fix `threads.c` function syntax for `coder_routine`.
- [ ] Pass valid thread args (`&sim->coders[i]` is the cleanest choice).
- [ ] Launch all coder threads and join all coder threads.
- [ ] Add monitor thread launch/join.
- [ ] Replace placeholder `printf` with logger calls.
- [ ] Make the routine use `coder->sim` instead of raw shared pointers where possible.

**Exit condition:** threads start, run loop skeleton, and exit cleanly.

**Do this immediately after logging is stable.**

---

## Step 7 — Implement heap queue fully

- [ ] In `heap.c`, implement `heap_init`, `heap_push`, `heap_pop`, `heap_peek`, `heap_remove_coder`, `heap_destroy`.
- [ ] Maintain `size` and `capacity` correctly.
- [ ] Add resize strategy when full.
- [ ] Do not pre-fill heap with all coders at init.
- [ ] Decide and document the comparison rule for FIFO and EDF.

**Exit condition:** per-dongle wait queue behaves as a correct min-heap.

**This must exist before you can trust dongle scheduling.**

---

## Step 8 — Implement dongle acquire/release with scheduling

- [ ] Implement waiting path for one dongle under dongle mutex.
- [ ] Push waiter with computed priority.
- [ ] Grant dongle only if waiter is top-priority and cooldown has elapsed.
- [ ] Implement release path (`is_taken`, `holder_id`, `released_at`, cond broadcast).
- [ ] Compose two-dongle acquisition/release in deadlock-safe order.

**Exit condition:** no deadlock; queue order respects FIFO/EDF.

**This depends on the heap and thread skeleton being solid.**

---

## Step 9 — Implement monitor and stop behavior

- [ ] Add monitor loop polling every `MONITOR_SLEEP_MS`.
- [ ] Detect burnout from `last_compile_start`.
- [ ] Detect all-completed when `compiles_required` reached.
- [ ] Set `is_running = 0` under `stop_mutex`.
- [ ] Wake blocked waiters (broadcast on all dongles).

**Exit condition:** simulation stops exactly once on burnout or completion.

**Do this after the scheduler flow works.**

---

## Step 10 — Add cleanup and error path safety

- [ ] Destroy all mutexes/cond vars that were initialized.
- [ ] Join threads before destroying sync primitives.
- [ ] Free all allocations (`args`, `coders`, `dongles`, heap entries, `sim`).
- [ ] Ensure early-failure cleanup path is leak-safe.

**Exit condition:** normal exit and failure exit both clean resources.

**Do this before final testing.**

---

## Step 11 — Validate in this exact order

- [ ] Compile with strict flags and zero warnings.
- [ ] Bad-input matrix: argc, non-numeric, out-of-range, bad scheduler.
- [ ] One-coder run.
- [ ] Multi-coder run.
- [ ] FIFO ordering check.
- [ ] EDF ordering check.
- [ ] Cooldown enforcement check.
- [ ] Burnout detection check.
- [ ] `compiles_required` completion check.
- [ ] Stress and leak checks.

---

## Step 12 — Final done checklist

- [ ] Header/API consistency complete.
- [ ] Parser/bootstrap robust.
- [ ] Time and logging correct.
- [ ] Threading + monitor stable.
- [ ] Heap and scheduler complete.
- [ ] Dongle synchronization correct.
- [ ] Stop logic and cleanup correct.
- [ ] Validation suite passed.
