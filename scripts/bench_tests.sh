#!/bin/bash


# Colors 
REDCOLOR='\033[1;31m'
YELLOWCOLOR='\033[1;33m'
CYANCOLOR='\033[1;36m'
GREENCOLOR='\033[1;32m'
WHITECOLOR='\033[1;37m'
BLUECOLOR='\033[1;34m'
NOCOLOR='\033[0m'

# File path for test sets
FPTS=scripts/testSets
# File path for test outputs
FPTO=tmp/test_results
mkdir tmp # create tmp folder


# Take a screen of docker stats
runDockerStats()
{
    local ALG="$1"      # Used algorithm
    local NODE="$2"     # Node count (3/5/10/15/20)

    sleep 45
    docker stats --no-stream >> $FPTO/$ALG/stats-$ALG-$NODE.txt
    echo "" >> $FPTO/$ALG/stats-$ALG-$NODE.txt
}

# Run one test
runTest()
{
    local ALG="$1"      # Used algorithm
    local CONF="$2"     # Used configuration file
    local TSET="$3"     # Used transaction set
    local NODE="$4"     # Node count (3/5/10/15/20)
    local RUN="$5"      # Round number (1-5)

    echo -e "${YELLOWCOLOR}Algorithm: $ALG, with $NODE nodes, round $RUN${NOCOLOR}"

    runDockerStats $ALG $NODE &

    ./scripts/setup.sh $ALG $CONF $TSET > $FPTO/$ALG/$ALG-$NODE-$RUN.txt &
    pid_cmd=$!
    wait $pid_cmd
    ./scripts/clean.sh
    sleep 1
}


# Run 5 tests for each algorithm with 3, 5, 10, 15 and 20 nodes
testAlg()
{
    local ALG="$1"

    # 3 nodes
    for i in {1..5}; do
        runTest $ALG $FPTS/3conf.txt $FPTS/3cmdScript.txt 3 "$i"
    done

    # 5 nodes
    for i in {1..5}; do
        runTest $ALG $FPTS/5conf.txt $FPTS/5cmdScript.txt 5 "$i"
    done

    # 10 nodes
    for i in {1..5}; do
        runTest $ALG $FPTS/10conf.txt $FPTS/10cmdScript.txt 10 "$i"
    done

    # 15 nodes
    for i in {1..5}; do
        runTest $ALG $FPTS/15conf.txt $FPTS/15cmdScript.txt 15 "$i"
    done

    # 20 nodes
    for i in {1..5}; do
        runTest $ALG $FPTS/20conf.txt $FPTS/20cmdScript.txt 20 "$i"
    done
}

# Set up folders
#mkdir $FPTO
#mkdir $FPTO/falcon1024
#mkdir $FPTO/falcon512
#mkdir $FPTO/dilithium5
#mkdir $FPTO/dilithium3
#mkdir $FPTO/dilithium2
#mkdir $FPTO/ed25519
#mkdir $FPTO/ecdsa

# Run tests
testAlg falcon1024
testAlg falcon512
testAlg dilithium5
#testAlg dilithium3
#testAlg dilithium2
#testAlg ed25519
#testAlg ecdsa