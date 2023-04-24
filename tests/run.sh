#!/bin/bash

RESET="\033[0m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BOLD="\033[1m"

ORIGINAL_BIN=nm
MY_BIN=./ft_nm

function execute_test {
    echo -ne "Running test ${BOLD}${YELLOW}$1${RESET}: "
    $ORIGINAL_BIN $1 | sort > original_output
    $MY_BIN $1 | sort > my_output
    diff -u original_output my_output > diff_output
    if [ $? -eq 0 ]; then
        echo -e "${BOLD}${GREEN}OK${RESET}"
    else
        echo -e "${BOLD}${RED}KO${RESET}"
        echo -e "${BOLD}${YELLOW}Diff:${RESET}"
        cat diff_output
    fi
    rm original_output my_output diff_output
}

find tests/bin/ -type f | while read test; do
    execute_test $test
done
