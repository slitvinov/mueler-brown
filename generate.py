import mueler_brown
import mmap
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-n",
                    "--nsteps",
                    type=int,
                    required=True,
                    help="Number of steps")
parser.add_argument("-p",
                    "--period",
                    type=int,
                    required=True,
                    help="Period of recording")
parser.add_argument("-o",
                    "--output",
                    type=str,
                    required=True,
                    help="Output binary file")
args = parser.parse_args()
import numpy as np

nbytes = 2 * args.nsteps * np.dtype("<f8").itemsize
with open(args.output, "wb") as f:
    f.seek(nbytes - 1, 0)
    f.write(b'\0')

with open(args.output, "r+b") as f:
    mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_WRITE)
    mueler_brown.trajectory(args.nsteps, args.period, mm)
    mm.flush()
