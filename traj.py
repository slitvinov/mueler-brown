import matplotlib.pylab as plt
import numpy as np
import mueler_brown
import mmap
import sys

dtype = np.dtype("<f8")
n = 100000
period = 1000
nbytes = 2 * n * dtype.itemsize
xlo, xhi = -1.5, 1.5
ylo, yhi = -1.0, 2.0
with open("traj.bin", "wb") as f:
    f.seek(nbytes - 1, 0)
    f.write(b'\0')
with open("traj.bin", "r+b") as f:
    mm = mmap.mmap(f.fileno(), 0, mmap.ACCESS_WRITE)
    mueler_brown.trajectory(n, period, mm)
x, y = np.ndarray((2, n), dtype, mm, order='F')
plt.axis("equal")
plt.xlim(xlo, xhi)
plt.ylim(ylo, yhi)
plt.plot(x, y, ',', alpha=0.25)
plt.savefig("traj.png", bbox_inches = "tight")
sys.stderr.write("traj.py: traj.bin\n")
sys.stderr.write("traj.py: traj.png\n")
