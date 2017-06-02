# Copyright Jacqueline Kay 2017
# Distributed under the MIT License.
# See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#!/usr/bin/env python3

import argparse
import csv
import os
import os.path
import subprocess
import sys
import time

parser = argparse.ArgumentParser("run benchmarks for string hashing")

parser.add_argument('E', type=str, nargs=1, help='The executable name.')
parser.add_argument('T', type=int, nargs=1, help='The number of times to run the executable.')
parser.add_argument('--output', dest='output', type=str, nargs=1, help='Folder to save output to.')

args = parser.parse_args(sys.argv[1:])

# Format is csv
# real user sys 
executable = args.E[0]
T = args.T[0]
output = args.output[0]

if os.path.exists(output):
    print("Warning: overwriting existing file " + output, file=sys.stderr)
else:
    out_dir = os.path.dirname(output)
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

samples = [None] * T

# Measure system time
for i in range(T):
    FNULL = open(os.devnull, 'w')
    start = time.time()
    subprocess.call([executable], stdout=FNULL)
    end = time.time()
    FNULL.close()
    samples[i] = end - start

with open(output, 'w') as f:
    writer = csv.writer(f)
    writer.writerows([[x] for x in samples])

average = sum(samples) / T

print("Mean for {}: {}".format(executable, average))
