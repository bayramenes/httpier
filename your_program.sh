#!/bin/sh

set -e # Exit early if any commands fail

(
  cd "$(dirname "$0")" 
  gcc -lcurl -lz -o /tmp/httpier app/*.c
)

exec /tmp/httpier "$@"
