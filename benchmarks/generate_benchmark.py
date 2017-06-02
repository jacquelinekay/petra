# Copyright Jacqueline Kay 2017
# Distributed under the MIT License.
# See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#!/usr/bin/env python3

import argparse
import em
import os
import os.path
import random
import string
import sys

parser = argparse.ArgumentParser("generate benchmarks for string hashing")

parser.add_argument('N', type=int, nargs=1, help='The number of strings in the hash set.')
parser.add_argument('M', type=int, nargs=1, help='The maximum string length.')
parser.add_argument('T', type=int, nargs=1, help='The number of times to execute one hash.')
parser.add_argument('--in_filename', dest='in_filename', type=str, nargs=1, help='Filename to save output to.')
parser.add_argument('--out_filename', dest='out_filename', type=str, nargs=1, help='Filename to save output to.')

args = parser.parse_args(sys.argv[1:])

N = args.N[0]
M = args.M[0]
T = args.T[0]
in_filename = args.in_filename[0]
out_filename = args.out_filename[0]

# Generate random strings
string_set = set()

while len(string_set) < N:
    generated = ''.join(random.choice(string.ascii_lowercase) for _ in range(M))
    # This reverse check is necessary for the simple string hash
    if not reversed(generated) in string_set:
        string_set.add(generated)

out_dir = os.path.dirname(out_filename)
if not os.path.exists(out_dir):
    os.makedirs(out_dir)

interpreter = em.Interpreter(output=open(out_filename, 'w+'))

interpreter.globals['string_set'] = string_set
interpreter.globals['T'] = T
interpreter.globals['N'] = N

interpreter.file(open(in_filename))
interpreter.shutdown()
