#! /bin/bash

prog=${1:?}
input=${2:?}
profile=${3:?}

echo "Running program '$prog' over input '$input' with profile '$3'..."

# simulate a random work duration...
sleep $(expr $RANDOM \% 3)  # wait up to 2 seconds

# simulate a possible 10% failure rate
if test $(expr $RANDOM \% 10) = 0; then
   echo "An error $RANDOM occurred!" >&2
   exit 1
fi
exit 0

