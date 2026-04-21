# Codexion Project Completion TODO (Step-by-Step)

Use this list in order. Each item below is implementation work that must be finished before final validation.

## 1) Normalize naming and public API

- [x] Rename typoed symbols consistently

Steps:
1. Rename `innit` to `init` in declarations, definitions, and all call sites.
2. Rename `innit_coders` to `init_coders` everywhere.
3. Rename `debbug` to `debug` everywhere.
4. Rebuild and fix any compile errors from renamed symbols.

- [x] Clean header declarations

Steps:
1. Remove duplicate function prototypes from the header.
2. Keep only public interfaces in the header.
3. Move internal-only helpers to their `.c` files as `static` functions where possible.

## 2) Fix Makefile structure

- [x] Implement standard targets and behavior

Steps:
1. Keep `all` building `Codexion`.
2. Make `clean` remove only object files.
3. Add `fclean` to remove binary after `clean`.
4. Set `re` to call `fclean` then `all`.
5. Run `make`, `make clean`, `make fclean`, and `make re` to verify behavior.

## 3) Correct thread lifecycle and stop flow

- [ ] Make monitor and workers exit deterministically

Steps:
1. In launch flow, create all worker threads first.
2. Create monitor thread immediately after workers start.
3. Join worker threads.
4. Join monitor thread.
5. Ensure no thread calls `exit()` directly.

- [ ] Centralize stop logic without freeing in worker context

Steps:
1. Keep `stop_simulation` responsible only for setting stop flag and waking waiters.
2. Protect stop flag writes with `stop_mutex`.
3. Broadcast all dongle condition variables during stop.
4. Remove any `free_all` or process termination calls from worker/monitor threads.
5. Perform cleanup once from the main control path after joins.

## 4) Fix coder routine sequencing

- [ ] Enforce valid action order and early exits

Steps:
1. Make `taken_dongle` return success/failure and check it in routine loop.
2. If compile step fails (burnout), stop current coder loop immediately.
3. Do not run debug/refactor after burnout.
4. Use local variable name `coder` (not `coders`) for clarity.

- [ ] Apply action timings

Steps:
1. Sleep for `time_to_compile` after compile begins.
2. Sleep for `time_to_debug` after debug begins.
3. Sleep for `time_to_refactor` after refactor begins.
4. Re-check stop condition between action phases.

## 5) Fix dongle wait/cooldown behavior

- [ ] Prevent cooldown deadlock

Steps:
1. Do not sleep while holding dongle mutex.
2. Keep cooldown enforcement by timestamp (`released_at`) checks.
3. Replace indefinite wait with timed wait or periodic wakeup in acquire loop.
4. Re-check stop flag and cooldown readiness after each wake.

- [ ] Prevent duplicate waiter entries and stale queue state

Steps:
1. Ensure each coder is queued once per acquire attempt.
2. Remove coder from queue when stopping or aborting acquire.
3. Verify queue size does not grow unbounded across repeated loops.

## 6) Implement correct monitor criteria

- [ ] Detect both terminal conditions safely

Steps:
1. Completion condition: all coders reached `compiles_required`.
2. Burnout condition: per coder, compare `now - last_compile_start` to `time_to_burnout`.
3. Read coder timing/counters under coder mutex to avoid races.
4. On terminal condition, call stop logic once and exit monitor thread.
5. Keep monitor polling with short sleep (`MONITOR_SLEEP_MS`) to avoid busy spin.

## 7) Harden parser and initialization

- [ ] Add robust allocation and input guards

Steps:
1. Validate each numeric input domain before starting threads.
2. Reject zero/negative values where invalid.
3. Check all allocations (`sim`, `args`, coders, dongles).
4. Prefer zero-initialized allocation for top-level structs.

- [ ] Add partial-init rollback

Steps:
1. If any init step fails after mutex creation starts, destroy initialized mutexes/conds.
2. Free partially allocated arrays safely.
3. Return clean failure without leaking resources.

## 8) Complete cleanup paths

- [ ] Destroy every initialized sync primitive

Steps:
1. Destroy each coder `last_compile_mutex`.
2. Destroy each dongle mutex and cond.
3. Destroy global `stop_mutex` and `log_mutex`.
4. Free waiters heap storage for every dongle.
5. Free `args`, coder array, dongle array, and sim object once.

## 9) Final code quality pass

- [ ] Remove race-prone direct shared-state access

Steps:
1. Audit all `is_running` reads/writes; route through lock-safe helpers.
2. Audit monitor reads of coder fields; lock where needed.
3. Ensure no mutex remains locked across function paths on error.

- [ ] Confirm no duplicate responsibility between monitor and coder paths

Steps:
1. Ensure burnout handling does not conflict with monitor stop handling.
2. Ensure only one path performs final shutdown transition.
3. Ensure logs for terminal events are emitted at most once.

## 10) Ready-for-validation gate

- [ ] Implementation complete

Steps:
1. Confirm all sections 1-9 are checked.
2. Rebuild with strict flags and zero warnings.
3. Only then move to runtime validation/test matrix.
