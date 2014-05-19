#!/bin/sh
#
# Borrowed from: https://bitbucket.org/mihaila/bintrace
#
set -e
aslrLocation="/proc/sys/kernel/randomize_va_space"
switch=$(cat "$aslrLocation")
case "$switch" in
 0) switch=2; echo "turning ASLR on.";;
 1|2|*) switch=0; echo "turning ASLR off.";;
esac

echo "$switch" > "$aslrLocation"

