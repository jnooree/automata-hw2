#!/bin/bash -eu

workdir="${1-test}/generated"
suffix="${2-.debug}"

rm -rf "$workdir"
scripts/generate_test.py "$workdir"
scripts/run_runner.sh "$workdir" "$suffix"
