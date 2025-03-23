import mmap
import sys
import os
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("inputs", nargs="+", help="Input binary files")
parser.add_argument("-o", "--output",                     required=True, help="Output image file")
args = parser.parse_args()

import matplotlib.pylab as plt
import numpy as np
dtype = np.dtype("<f8")
itemsize = dtype.itemsize
plt.axis("equal")
plt.xlim(-1.5, 1.5)
plt.ylim(-1.0, 2.0)
ntotal = 0
for fname in args.inputs:
    filesize = os.path.getsize(fname)
    n = filesize // (2 * itemsize)
    with open(fname, "rb") as f:
        mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
    x, y = np.ndarray((2, n), dtype=dtype, buffer=mm, order='F')
    plt.plot(x, y, ',b', alpha=0.25)
    ntotal += n
plt.savefig(args.output, bbox_inches="tight")
sys.stderr.write(f"view.py: {args.output}: {ntotal} points\n")
