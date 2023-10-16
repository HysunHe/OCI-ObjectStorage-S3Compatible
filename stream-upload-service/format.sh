#!/usr/bin/env bash
# YAPF formatter, adapted from ray.
#
# Usage:
#    # Do work and commit your work.

#    # Format files that differ from origin/master.
#    bash format.sh

#    # Commit changed files with message 'Run yapf and pylint'
#
#
# YAPF + Clang formatter (if installed). This script formats all changed files from the last mergebase.
# You are encouraged to run this locally before pushing changes for review.

# Cause the script to exit if a single command fails
set -eo pipefail

# this stops git rev-parse from failing if we run this from the .git directory
builtin cd "$(dirname "${BASH_SOURCE:-$0}")"
# ROOT="$(git rev-parse --show-toplevel)"
# builtin cd "$ROOT" || exit 1

YAPF_VERSION=$(yapf --version | awk '{print $2}')

PYLINT_VERSION=$(pylint --version | head -n 1 | awk '{print $2}')

PYLINT_QUOTES_VERSION=$(pip list | grep pylint-quotes | awk '{print $2}')

# params: tool name, tool version, required version
tool_version_check() {
    echo "p1=$1, p2=$2, p3=$3"
    if [[ $2 != $3 ]]; then
        echo "Wrong $1 version installed: $3 is required, not $2."
        exit 1
    fi
}

tool_version_check "yapf" $YAPF_VERSION "$(grep "yapf==" requirements-dev.txt | cut -d'=' -f3)"
tool_version_check "pylint" $PYLINT_VERSION "$(grep "pylint==" requirements-dev.txt | cut -d'=' -f3)"
tool_version_check "pylint-quotes" $PYLINT_QUOTES_VERSION "$(grep "pylint-quotes==" requirements-dev.txt | cut -d'=' -f3)"

PYTHON_FILES=$(find . -name '*.py' ! -path './build/**' ! -path './tests/**' -print0 |  xargs -0) 

YAPF_FLAGS=(
    '--recursive'
    '--parallel'
)

YAPF_EXCLUDES=(
    '--exclude' 'build/**'
    '--exclude' 'tests/**'
)

# Format all files
yapf --in-place "${YAPF_FLAGS[@]}" "${YAPF_EXCLUDES[@]}" $PYTHON_FILES

echo 'yapf: Done'

# Run Pylint
echo 'Pylint:'
pylint --load-plugins pylint_quotes $PYTHON_FILES
