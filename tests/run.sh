#!/bin/bash

RESET="\033[0m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BOLD="\033[1m"

ORIGINAL_BIN=nm
MY_BIN=./ft_nm

error=0

function execute_test_sort {
    echo -ne "Running test (sorted) ${BOLD}${YELLOW}nm $2 $1${RESET}: "
    # if $2 is not empty
    if [ ! -z "$2" ]; then
        $ORIGINAL_BIN $2 $1 | sort > original_output
        $MY_BIN $2 $1 | sort > my_output
    else
        $ORIGINAL_BIN $1 | sort > original_output
        $MY_BIN $1 | sort > my_output
    fi
    diff -u original_output my_output > diff_output
    if [ $? -eq 0 ]; then
        echo -e "${BOLD}${GREEN}OK${RESET}"
    else
        error=1
        echo -e "${BOLD}${RED}KO${RESET}"
        echo -e "${BOLD}${YELLOW}Diff:${RESET}"
        cat diff_output
    fi
    rm original_output my_output diff_output
}

function execute_test {
    echo -ne "Running test ${BOLD}${YELLOW}nm $2 $1${RESET}: "
    # if $2 is not empty
    if [ ! -z "$2" ]; then
        $ORIGINAL_BIN $2 $1 > original_output
        $MY_BIN $2 $1 > my_output
    else
        $ORIGINAL_BIN $1 > original_output
        $MY_BIN $1 > my_output
    fi
    diff -u original_output my_output > diff_output
    if [ $? -eq 0 ]; then
        echo -e "${BOLD}${GREEN}OK${RESET}"
    else
        error=1
        echo -e "${BOLD}${RED}KO${RESET}"
        echo -e "${BOLD}${YELLOW}Diff:${RESET}"
        cat diff_output
    fi
    rm original_output my_output diff_output
}

find tests/bin/ -type f | while read test; do
    execute_test_sort $test
    execute_test_sort $test -g
    execute_test_sort $test -u
    execute_test_sort $test -r
    execute_test $test -p
done

exit $error
