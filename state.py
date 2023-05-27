import matplotlib.pylab as plt
import numpy as np
import mueler_brown
import mmap

xlo, xhi = -1.5, 1.5
ylo, yhi = -1.0, 2.0
xrange = np.linspace(xlo, xhi, 200)
yrange = np.linspace(ylo, yhi, 200)
for x in xrange:
    for y in yrange:
        s = mueler_brown.state(x, y)
        if s != 2:
            print(x, y, s)
