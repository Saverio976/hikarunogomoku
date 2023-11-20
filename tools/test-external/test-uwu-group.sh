#!/bin/bash

executable="$1"

# check executable is given
if [[ -z "$executable" ]]; then
    echo "Usage: $0 <executable>"
    exit 1
fi

# $1: element
# $@: array
contains() {
    local element="$1"
    shift
    for item in "$@"; do
        [[ "$element" == "$item" ]] && return 0;
    done
    return 1
}

# $1: name
# $2: input
# ${@:3}: expected
test() {
    echo -n "" > res.txt
    echo "testing $1..."
    echo -e "$2" | "$executable" > res.txt &
    PID=$!
    while [[ "$(cat res.txt)" == "" ]]; do
        sleep 0.1
    done
    kill -9 $PID &>/dev/null
    # output=$(echo -e "$2" | "$executable")
    output=$(cat res.txt)
    got="${output##*$'\n'}"
    echo "comparing results..."

    if contains $got ${@:3}; then
        echo -e "  * \033[32mOK $1\033[0m"
    else
        echo -e "  * \033[31mKO $1 (expected ${@:3} but got $got)\033[0m"
    fi
    rm res.txt
}

echo -e "\n----- GOMOKU UwU TESTS -----\n"

# -------- DEFENSE -------- #

test "defense horizontal three" "
INFO timeout_turn 5000
START 20
BOARD
0,0,2
1,0,2
2,0,2
DONE" "3,0"

test "defense horizontal three" "
INFO timeout_turn 5000
START 20
BOARD
11,9,2
12,9,2
13,9,2
DONE" "10,9" "14,9"

test "defense horizontal four" "
INFO timeout_turn 5000
START 20
BOARD
0,0,2
1,0,2
2,0,2
3,0,2
DONE" "4,0"

test "defense horizontal broken four" "
INFO timeout_turn 5000
START 20
BOARD
8,6,2
10,6,2
11,6,2
12,6,2
DONE" "9,6"

test "defense horizontal broken four at middle" "
INFO timeout_turn 5000
START 20
BOARD
8,6,2
9,6,2
11,6,2
12,6,2
DONE" "10,6"

test "defense vertical three" "
INFO timeout_turn 5000
START 20
BOARD
0,0,2
0,1,2
0,2,2
DONE" "0,3"

test "defense vertical four" "
INFO timeout_turn 5000
START 20
BOARD
0,0,2
0,1,2
0,2,2
0,3,2
DONE" "0,4"

test "defense vertical broken four" "
INFO timeout_turn 5000
START 20
BOARD
3,2,2
3,4,2
3,5,2
3,6,2
DONE" "3,3"

test "defense vertical broken four at middle" "
INFO timeout_turn 5000
START 20
BOARD
3,2,2
3,3,2
3,5,2
3,6,2
DONE" "3,4"

test "defense diagonal down three" "
INFO timeout_turn 5000
START 20
BOARD
0,0,2
1,1,2
2,2,2
DONE" "3,3"

test "defense diagonal down broken three" "
INFO timeout_turn 5000
START 20
BOARD
8,5,2
9,6,2
11,8,2
DONE" "10,7"

test "defense diagonal down broken four at middle" "
INFO timeout_turn 5000
START 20
BOARD
7,4,2
8,5,2
10,7,2
11,8,2
DONE" "9,6"

test "defense diagonal up broken four" "
INFO timeout_turn 5000
START 20
BOARD
7,7,2
9,9,2
10,10,2
11,11,2
DONE" "8,8"

test "defense diagonal down four" "
INFO timeout_turn 5000
START 20
BOARD
0,0,1
2,2,2
3,3,2
4,4,2
5,5,2
DONE" "6,6" "1,1"

test "defense diagonal up three" "
INFO timeout_turn 5000
START 20
BOARD
4,0,2
3,1,2
2,2,2
DONE" "1,3" "0,4"

test "specific diagonal up three in middle of the board" "
INFO timeout_turn 5000
START 20
BOARD
3,3,2
2,4,2
1,5,2
DONE" "4,2" "0,6"

test "defense diagonal up four" "
INFO timeout_turn 5000
START 20
BOARD
4,0,2
3,1,2
2,2,2
1,3,2
DONE" "0,4"

test "defense diagonal up four in middle of the board" "
INFO timeout_turn 5000
START 20
BOARD
5,1,1
4,2,2
3,3,2
2,4,2
1,5,2
DONE" "0,6"

test "defense diagonal up four in middle of the board" "
INFO timeout_turn 5000
START 20
BOARD
0,6,1
4,2,2
3,3,2
2,4,2
1,5,2
DONE" "5,1"

# -------- ATTACK -------- #

test "attack horizontal four" "
INFO timeout_turn 5000
START 20
BOARD
12,8,1
13,8,1
14,8,1
15,8,1
DONE" "11,8" "16,8"

test "attack horizontal broken four at middle" "
INFO timeout_turn 5000
START 20
BOARD
6,3,1
7,3,1
9,3,1
10,3,1
DONE" "8,3"

test "attack horizontal choice" "
INFO timeout_turn 5000
START 20
BOARD
10,5,2
10,6,2
10,7,2
10,8,2
1,7,1
2,7,1
3,7,1
4,7,1
DONE" "5,7" "0,7"

test "attack horizontal choice 3" "
INFO timeout_turn 5000
START 20
BOARD
10,5,2
10,6,2
10,7,2
10,8,2
9,4,2
8,4,2
7,4,2
6,4,2
1,7,1
2,7,1
3,7,1
4,7,1
DONE" "0,7" "5,7"

test "attack vertical choice" "
INFO timeout_turn 5000
START 20
BOARD
10,5,1
10,6,1
10,7,1
10,8,1
1,7,2
2,7,2
3,7,2
4,7,2
DONE" "10,4" "10,9"

# -------- BOARD BORDERS -------- #

test "board right border" "
INFO timeout_turn 5000
START 20
BOARD
19,0,2
19,1,2
19,2,2
DONE" "19,3" "19,4"

test "board bottom border" "
INFO timeout_turn 5000
START 20
BOARD
0,19,2
1,19,2
2,19,2
DONE" "3,19" "4,19"

# -------- SPECIFIC CASES -------- #

test "specific case 1" "

INFO timeout_turn 5000
START 20
BOARD
10,10,2
10,11,2
11,11,1
DONE" "10,11" "10,9" "10,12"

test "specific case 2" "
INFO timeout_turn 5000
START 20
BOARD
16,10,2
17,10,2
18,10,2
DONE" "15,10" "19,10"

test "specific case 3" "
INFO timeout_turn 5000
START 20
BOARD
8,10,1
10,10,2
11,10,2
12,10,2
DONE" "9,10" "13,10"

echo -e "\n----------------------------\n"
