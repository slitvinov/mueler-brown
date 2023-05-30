import matplotlib.pylab as plt
import numpy as np
import mueler_brown
import sys

argv = sys.argv[:]
argv.pop(0)
dtype = np.dtype("<f8, <i4", align=True)
if len(argv) == 0:
    n = 10000000
    buf = bytearray(n * dtype.itemsize)
    mueler_brown.states(n, buf)
    with open("states.bin", "wb") as f:
        f.write(buf)
else:
    with open(argv[0], "rb") as f:
        buf = f.read()
    n = len(buf) // dtype.itemsize
S = np.ndarray(n, dtype, buffer=buf, order='F')
T01 = S["f0"][S["f1"] == 1]
T10 = S["f0"][S["f1"] == 0]
plt.hist(np.log10(T01), alpha=0.5, bins=50, density=True)
plt.hist(np.log10(T10), alpha=0.5, bins=50, density=True)
plt.savefig("states.png")
