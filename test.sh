#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="$ROOT_DIR/codexion"
VALGRIND_BIN=""

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PASS_COUNT=0
FAIL_COUNT=0
SKIP_COUNT=0

log() {
    printf '%b%s%b\n' "$2" "$1" "$NC"
}

pass() {
    PASS_COUNT=$((PASS_COUNT + 1))
    log "[PASS] $1" "$GREEN"
}

fail() {
    FAIL_COUNT=$((FAIL_COUNT + 1))
    log "[FAIL] $1" "$RED"
}

skip() {
    SKIP_COUNT=$((SKIP_COUNT + 1))
    log "[SKIP] $1" "$YELLOW"
}

section() {
    log "\n== $1 ==" "$BLUE"
}

build_project() {
    section "Build"
    make -C "$ROOT_DIR" clean >/dev/null
    if make -C "$ROOT_DIR" >/dev/null; then
        pass "make clean && make"
    else
        fail "build failed"
        exit 1
    fi
}

run_expect_success() {
    local name="$1"
    local timeout_seconds="$2"
    shift 2

    local output
    local status=0
    if output="$(timeout "$timeout_seconds" "$BIN" "$@" 2>&1)"; then
        status=0
    else
        status=$?
    fi

    if [[ $status -eq 0 ]]; then
        pass "$name"
    else
        printf '%s\n' "$output"
        fail "$name (exit $status)"
    fi
}

run_expect_failure() {
    local name="$1"
    local timeout_seconds="$2"
    shift 2

    local output
    local status=0
    if output="$(timeout "$timeout_seconds" "$BIN" "$@" 2>&1)"; then
        status=0
    else
        status=$?
    fi

    if [[ $status -ne 0 ]]; then
        pass "$name"
    else
        printf '%s\n' "$output"
        fail "$name (expected failure, got success)"
    fi
}

find_valgrind() {
    if command -v valgrind >/dev/null 2>&1; then
        VALGRIND_BIN="valgrind"
    else
        VALGRIND_BIN=""
    fi
}

run_valgrind_check() {
    local name="$1"
    local timeout_seconds="$2"
    shift 2

    if [[ -z "$VALGRIND_BIN" ]]; then
        skip "$name (valgrind not installed)"
        return 0
    fi

    local log_file
    log_file="$(mktemp)"
    local status=0

    if timeout "$timeout_seconds" "$VALGRIND_BIN" \
        --leak-check=full \
        --show-leak-kinds=all \
        --errors-for-leak-kinds=definite,indirect,possible \
        --error-exitcode=42 \
        "$BIN" "$@" >"$log_file" 2>&1; then
        status=0
    else
        status=$?
    fi

    if [[ $status -eq 0 ]]; then
        pass "$name"
    else
        printf '%s\n' "----- valgrind output for: $name -----"
        sed -n '1,120p' "$log_file"
        printf '%s\n' "----- end valgrind output -----"
        fail "$name (exit $status)"
    fi

    rm -f "$log_file"
}

main() {
    build_project
    find_valgrind

    section "Valid Runs"
    run_expect_success "2 coders, fifo, 1 compile" 10 2 800 50 50 50 1 0 fifo
    run_expect_success "1 coder burnout" 10 1 300 50 50 50 1 0 fifo
    run_expect_success "5 coders, fifo" 15 5 500 30 30 30 2 0 fifo
    run_expect_success "4 coders, edf" 15 4 600 40 40 40 2 0 edf

    section "Invalid Inputs"
    run_expect_failure "missing arguments" 5 2 800 50 50 50 1 0
    run_expect_failure "negative coder count" 5 -1 800 50 50 50 1 0 fifo
    run_expect_failure "zero coder count" 5 0 800 50 50 50 1 0 fifo
    run_expect_failure "negative burnout time" 5 2 -1 50 50 50 1 0 fifo
    run_expect_failure "zero compiles required" 5 2 800 50 50 50 0 0 fifo
    run_expect_failure "negative cooldown" 5 2 800 50 50 50 1 -1 fifo
    run_expect_failure "invalid scheduler" 5 2 800 50 50 50 1 0 round-robin

    section "Leak Checks"
    run_valgrind_check "2 coders leak check" 20 2 800 50 50 50 1 0 fifo
    run_valgrind_check "1 coder leak check" 15 1 300 50 50 50 1 0 fifo

    section "Stress"
    run_expect_success "20 coders stress" 20 20 600 20 20 20 3 0 fifo

    printf '\nSummary: %d passed, %d failed, %d skipped\n' "$PASS_COUNT" "$FAIL_COUNT" "$SKIP_COUNT"
    [[ $FAIL_COUNT -eq 0 ]]
}

main "$@"
