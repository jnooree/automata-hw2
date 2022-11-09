#!/bin/bash -eu

workdir="${1-test}/generated"
suffix="${2-.debug}"

rm -rf "$workdir"
scripts/generate_test.py "$workdir"
cp -ft "$workdir" test/*.{q,a}

scripts/run_runner.sh "$workdir" "$suffix"
