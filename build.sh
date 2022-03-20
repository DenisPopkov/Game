#!/bin/bash
# file: build.sh
echo -e "\Wordle Game"
echo
echo [+] Attempting to compile game...
echo gcc cw.c -Wall -Wextra -o cw
gcc cwordle.c -Wall -Wextra -o cw
echo
echo -e "Run"