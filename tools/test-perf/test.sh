#!/bin/bash

test_binary() {
    OPONENT="$1"

    cat > config.yml << EOF
debug:
  board: true
  enable: true
player2:
  limits:
    memory: 70000000
    time: 5000
  exe: pbrain-hikarunogomoku
player1:
  limits:
    time: 5000
    memory: 70000000
  exe: $OPONENT
general:
  headless: true
EOF
    ./bonus/bin-external/liskvork > /dev/null
    RES="$?"

    echo -e "\n----------------------------"
    echo "-------------- $OPONENT"
    echo -e "----------------------------"
    echo "-Player1: $OPONENT"
    echo "-Player2: pbrain-hikarunogomoku"
    if [[ "$RES" == "1" ]]; then
        echo "-Win: $OPONENT"
    else
        echo "-Win: pbrain-hikarunogomoku"
    fi
    echo -e "--- STATS"
    cat stats.txt

    cat > config.yml << EOF
debug:
  board: true
  enable: true
player2:
  limits:
    memory: 70000000
    time: 5000
  exe: $OPONENT
player1:
  limits:
    time: 5000
    memory: 70000000
  exe: pbrain-hikarunogomoku
general:
  headless: true
EOF
    ./bonus/bin-external/liskvork > /dev/null
    RES="$?"

    echo -e "\n----------------------------"
    echo "-------------- $OPONENT"
    echo -e "----------------------------"
    echo "-Player1: pbrain-hikarunogomoku"
    echo "-Player2: $OPONENT"
    if [[ "$RES" == "1" ]]; then
        echo "-Win: pbrain-hikarunogomoku"
    else
        echo "-Win: $OPONENT"
    fi
    echo -e "--- STATS"
    cat stats.txt
}

compil_our_binary() {
    cmake -S . -B build -DWITH_STAT=ON
    cmake --build build --parallel
}

compil_our_binary

for op in ./bonus/bin-external/pbrain-gomoku-ai-*; do
    test_binary "$op"
done
