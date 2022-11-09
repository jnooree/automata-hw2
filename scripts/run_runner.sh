#!/bin/bash -eu

. scripts/common_functions.sh

run_tests "run$2" "$1" q o
check_results "$1"
