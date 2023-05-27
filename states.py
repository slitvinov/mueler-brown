import matplotlib.pylab as plt
import numpy as np
import mueler_brown
import mmap

dtype = np.dtype("<f8, <i4", align=True)
n = 1_000_000
buf = bytearray(n * dtype.itemsize)
mueler_brown.states(n, buf)
S = np.ndarray(n, dtype, buffer=buf, order='F')
T01 = S["f0"][S["f1"] == 1]
T10 = S["f0"][S["f1"] == 0]
plt.hist(np.log(T01), alpha=0.5, bins=100, density=True)
plt.hist(np.log(T10), alpha=0.5, bins=100, density=True)
plt.savefig("states.png")
