import matplotlib.pylab as plt
import numpy as np
import mueler_brown

n = 100000
period = 1
x, y = np.ndarray((2, n), buffer=mueler_brown.trajectory(n, period), order='F')
plt.plot(x, y, '-x', alpha=0.1)
plt.show()
