import matplotlib.pylab as plt
import numpy as np
import mueler_brown
import mmap

dtype = np.dtype("<f8, <i4", align=True)
n = 1_000_000
buf = bytearray(n * dtype.itemsize)
mueler_brown.states(n, buf)
S = np.ndarray(n, dtype, buffer=buf, order='F')
