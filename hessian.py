import mueler_brown

x = 0.25
y = 0.5
d = 0.001
H = mueler_brown.hessian(x, y)
hxx = (mueler_brown.force(x + d, y)[0] - mueler_brown.force(x - d, y)[0]) / (2 * d)
hxy = (mueler_brown.force(x, y + d)[0] - mueler_brown.force(x, y - d)[0]) / (2 * d)
hyy = (mueler_brown.force(x, y + d)[1] - mueler_brown.force(x, y - d)[1]) / (2 * d)
print(hxx, H[0][0])
print(hxy, H[0][1], H[1][0])
print(hyy, H[1][1])
