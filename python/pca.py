import matplotlib.pylab as plt
import numpy as np
import mueler_brown
import sklearn.decomposition
import mmap

n = 10_000
period = 1_000
nbytes = 2 * n * 8
with open("o.bin", "wb") as f:
    f.seek(nbytes - 1, 0)
    f.write(b'\0')
with open("o.bin", "r+b") as f:
    mm = mmap.mmap(f.fileno(), nbytes, mmap.ACCESS_WRITE)
    mueler_brown.trajectory(n, period, mm)
X = np.ndarray((n, 2), buffer=mm)
pca = sklearn.decomposition.PCA(n_components=1)
pca.fit(X)
Y = pca.inverse_transform(pca.transform(X))
plt.plot(*X.T, ',')
plt.plot(*Y.T, ',')
plt.show()
