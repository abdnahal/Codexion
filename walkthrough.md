# Codexion Remaining To-Do (Updated)

Current status:
- `make` currently builds successfully.
- Core dongle queue helpers and heap operations exist.
- Major initialization bugs from earlier are mostly fixed.
- Current blocker: runtime still hangs in single-coder mode and does not terminate cleanly.
- Monitor code now exists but is wired in the wrong lifecycle order.

Use this checklist in order.

---

## 0) Immediate fixes for current hang (do this first)

Observed behavior:
- With 1 coder, program prints first cycle then hangs.
- With multiple coders, threads can run forever because stop flag is never flipped.

Root causes and required fixes:
- [ ] Start monitor before joining coder threads (currently monitor starts after coder joins, so it cannot stop them).
- [ ] In monitor shutdown path, set `sim->is_running = 0` under `stop_mutex` and broadcast every dongle cond var.
- [ ] Stop condition must use per-coder burnout logic (`last_compile_start + time_to_burnout`), not only `now - start_time`.
- [ ] Monitor currently busy-spins; add a small sleep (`MONITOR_SLEEP_MS`) to avoid CPU burn.
- [ ] Fix 1-coder cooldown wait path: avoid waiting forever on `pthread_cond_wait` when no other thread can broadcast.
- [ ] Use timed waits or a recheck loop for cooldown so a single coder can progress after cooldown expires.
- [ ] In `coder_routine`, use action return values and break when `compile` fails (burnout).
- [ ] Add actual action timing (`time_to_compile`, `time_to_debug`, `time_to_refactor`) so simulation advances instead of hot-spinning at ~0 ms.
- [ ] Protect monitor reads of coder state (`compile_count`/timing fields) with existing mutexes to avoid data races.

Exit when:
- 1-coder run no longer deadlocks.
- simulation terminates deterministically on burnout or completion.
- joins return without manual interruption.

---

## 1) API and naming consistency (still pending)

- [ ] Rename `innit`/`innit_coders` to `init`/`init_coders` everywhere.
- [ ] Rename `debbug` to `debug` everywhere.
- [ ] In `codexion.h`, remove duplicate prototype declarations (`taken_dongle` appears twice).
- [ ] In `codexion.h`, remove internal-only helper prototypes from public header if used only in one `.c` file.

Exit when: one naming style is used everywhere and header is clean.

---

## 2) Makefile final fixes (partially done)

- [x] Build command uses `-Wall -Wextra -Werror -pthread` and outputs `Codexion`.
- [ ] `clean` currently removes binary; align with standard split (`clean` objects only, `fclean` removes binary).
- [ ] Fix `re` target to call make targets, not shell commands.
- [ ] Add `fclean` target.
- [ ] Keep `clean` for object files and `fclean` for binary.

Suggested shape:
- `all: $(NAME)`
- `clean: rm -f *.o`
- `fclean: clean` then remove `$(NAME)`
- `re: fclean all`

Exit when: `make re` works reliably.

---

## 3) Thread loop correctness (important)

- [x] `coder_routine` now loops while simulation is running.
- [ ] `coder_routine` still ignores return values from action calls.
- [ ] Check and use return values from `taken_dongle`, `compile`, `debug`, `refactor`.
- [ ] Break loop if `compile` returns 0.
- [ ] Avoid continuing with debug/refactor after burnout.
- [ ] Rename local variable from `coders` to `coder` for clarity.

Exit when: loop exits correctly and actions run in valid order only.

---

## 4) Stop logic and monitor (missing)

- [~] Monitor thread function exists, but lifecycle/order is incorrect.
- [ ] Start monitor before joining coder threads and join it on shutdown.
- [ ] On burnout or all-complete, set `sim->is_running = 0` under `stop_mutex`.
- [ ] Broadcast all dongle cond vars so blocked coders wake up and exit.
- [ ] Rewrite stop criteria to avoid premature/global burnout checks.
- [ ] Prevent duplicate/late stop transitions so terminal events are logged once.

Exit when: simulation terminates on defined conditions (not infinite loop).

---

## 5) Logging behavior cleanup (mostly done)

- [x] `log_print` is centralized and protected by `log_mutex`.
- [x] `sim_is_running` checks `is_running` under `stop_mutex`.
- [ ] Ensure monitor stop transition also respects `stop_mutex` (currently direct write risk).
- [ ] Ensure terminal event logs are printed exactly once on shutdown.
- [ ] Decide whether burnout log should print even after stop flag flips.

Exit when: logs are coherent and deterministic at shutdown.

---

## 6) Parser/init robustness (partially done)

- [x] coder mutexes and state fields are initialized.
- [x] dongle mutex/cond are initialized.
- [x] `is_running` and `start_time` are initialized.
- [ ] Add allocation failure checks for `sim` and `sim->args`.
- [ ] Zero-init `sim` safely (`calloc` preferred).
- [ ] Validate numeric domains (non-zero/positive timings, coder count > 0).

Exit when: init path is safe on all invalid input and allocation failures.

---

## 7) Cleanup completeness (still pending)

- [ ] In `free_all`, destroy each coder `last_compile_mutex`.
- [ ] Destroy global `stop_mutex` and `log_mutex` before freeing `sim`.
- [ ] Add partial-init cleanup path if init fails after some mutexes are created.

Exit when: no invalid mutex lifetime and no leaks on normal/early exit.

---

## 8) Scheduler correctness checks (needs validation)

- [x] Heap push/pop/peek/remove are implemented.
- [ ] Verify waiter enqueue is not duplicated across wakeups.
- [ ] Verify FIFO ordering under contention.
- [ ] Verify EDF ordering under contention.
- [ ] Verify cooldown behavior with repeated acquire/release.

Exit when: queue behavior matches policy in repeated runs.

---

## 9) Validation runbook (what is left to run)

- [ ] `make re`
- [ ] Wrong argc
- [ ] Non-numeric argument
- [ ] Bad scheduler string
- [ ] Zero/negative timing arguments
- [ ] 1 coder run
- [ ] 2 coder run
- [ ] 4+ coder run (ensure all IDs appear repeatedly)
- [ ] Burnout scenario
- [ ] `compiles_required` completion scenario
- [ ] FIFO and EDF comparison scenario
- [ ] Leak/sanitizer run

Suggested quick smoke set after hang fixes:
- [ ] 1 coder, non-zero cooldown (currently hangs after first cycle)
- [ ] 2 coders, FIFO, short times (must terminate)
- [ ] 2 coders, EDF, short times (must terminate)
- [ ] burnout case with very small `time_to_burnout`
- [ ] completion case with low `compiles_required`

Exit when: all above pass without deadlock, crash, or hang.

---

## Done when

- [ ] Consistent API names and clean header.
- [ ] Deterministic stop/monitor behavior exists.
- [ ] Thread loop obeys action return values.
- [ ] Cleanup destroys all initialized mutexes/conds.
- [ ] Validation runbook passes end to end.
