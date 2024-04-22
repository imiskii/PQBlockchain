
# This script will grep important values from bench testing and put it into one .txt file.
# The first argument of program is algorithm name
# The second argument of the program is folder with files that will be analyzed and from which will be created the output file.
# Also do not forget to adjust constants of this script if the testing scenario change (for example number of nodes or rounds execution)

# example: python3 statValGrep.py falcon512 tmp/test_results/

import re
import sys
from pathlib import Path

NODES=[3,5,10,15,20]
ROUNDS=5

def grepPerformance(file, stats, n_cnt, round, out_file):
    with open(file, 'r') as file:
        text = file.read()

    # Grep transmitted data value
    bytes_processed_values = re.findall(r'The program has processed \(send/receive\) (\d+) of bytes in messages\.', text)
    # Grep CPU cycles value
    cycles_values = re.findall(r'(\d+)\s+cycles', text)

    with open(out_file, 'a') as file:
        file.write("\n" + "Round: " + str(round) + "\n")
        for i in range(n_cnt):
            stats_index = i + ((round-1) * n_cnt)
            file.write(cycles_values[i] + " " + stats[n_cnt][stats_index] + " " + bytes_processed_values[i] + "\n")
    


def grepMemAllocation(file) -> list[str]:
    with open(file, 'r') as file:
        text = file.read()

    # Grep memory data
    bytes_allocated = re.findall(r'(\d+(?:\.\d+)?)[a-zA-Z]{3}\s\/', text)
    bytes_allocated = [rec.replace(".", ",") for rec in bytes_allocated]
    return bytes_allocated



if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 statValGrep.py <algorithm_name> <path_to_folder_with_testing_outputs>")
        sys.exit(1)

    alg_name = sys.argv[1]
    directory = sys.argv[2]

    if directory[-1] != "/":
        directory += "/"

    # Iterate stats files and save them for later
    stats = {}
    for n_cnt in NODES:
        file = directory + "stats-" + alg_name + "-" + str(n_cnt) + ".txt"
        stats[n_cnt] = grepMemAllocation(file)

    # Iterate files output files
    for n_cnt in NODES:
        for round in range(1,ROUNDS+1):
            file = directory + alg_name + "-" + str(n_cnt) + "-" + str(round) + ".txt"
            out_file = directory + alg_name + "-" + str(n_cnt) + ".out"
            grepPerformance(file, stats, n_cnt, round, out_file)

