#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <math.h>

static double E(double, double);
static void F(double, double, double *, double *);
static PyObject *energy(PyObject *self, PyObject *args) {
  double x, y;
  if (!PyArg_ParseTuple(args, "dd", &x, &y))
    return NULL;
  return PyFloat_FromDouble(E(x, y));
}

static PyObject *force(PyObject *self, PyObject *args) {
  double x, y, u, v;
  if (!PyArg_ParseTuple(args, "dd", &x, &y))
    return NULL;
  F(x, y, &u, &v);
  return Py_BuildValue("dd", u, v);
}

static PyObject *trajectory(PyObject *self, PyObject *args) {
  long n, i, j, k, period;
  double x, y, dt, gamma, vx, vy, rx, ry, fx, fy, kT, coeff, *out;
  gsl_rng *rnd;
  PyByteArrayObject *res = NULL;
  if (!PyArg_ParseTuple(args, "ll", &n, &period))
    return NULL;
  res = (PyByteArrayObject *)PyByteArray_FromStringAndSize(
      NULL, 2 * n * sizeof(double));
  out = (double *)res->ob_bytes;
  gsl_rng_env_setup();
  rnd = gsl_rng_alloc(gsl_rng_default);
  kT = 15;
  gamma = 1;
  dt = 0.01;
  x = -5.5822363449547829e-01;
  y = +1.4417258420221519e+00;
  vx = 0;
  vy = 0;
  coeff = sqrt(2 * kT * gamma * dt);
  j = 0;
  k = 0;
  for (i = 0;; i++) {
    if (j == 0 || j == period) {
      out[2 * k] = x;
      out[2 * k + 1] = y;
      k++;
      if (PyErr_CheckSignals() != 0)
        break;
      if (k == n)
        break;
      j = 0;
    }
    j++;
    F(x, y, &fx, &fy);
    rx = gsl_ran_gaussian(rnd, 1);
    ry = gsl_ran_gaussian(rnd, 1);
    vx += dt * (-fx - gamma * vx) + coeff * rx;
    vy += dt * (-fy - gamma * vy) + coeff * ry;
    x += dt * vx;
    y += dt * vy;
  }
  gsl_rng_free(rnd);
  return (PyObject *)res;
}
static PyMethodDef Mueler_BrownMethods[] = {
    {"energy", energy, METH_VARARGS, PyDoc_STR("Return energy")},
    {"force", force, METH_VARARGS, PyDoc_STR("Return force")},
    {"trajectory", trajectory, METH_VARARGS,
     PyDoc_STR("Returns paritcle trajectory")},
    {NULL, NULL, 0, NULL},
};
static PyModuleDef mueler_brownmodule = {
    PyModuleDef_HEAD_INIT,
    "mueler_brown",
    PyDoc_STR("Mueler-Brown potential"),
    -1,
    Mueler_BrownMethods,
};

PyMODINIT_FUNC PyInit_mueler_brown(void) {
  return PyModule_Create(&mueler_brownmodule);
}

static const double A[] = {-200, -100, -170, 15}, a[] = {-1, -1, -6.5, 0.7},
                    b[] = {0, 0, 11, 0.6}, c[] = {-10, -10, -6.5, 0.7},
                    x0[] = {1, 0, -0.5, -1}, y00[] = {0, 0.5, 1.5, 1.0};
static double E(double x, double y) {
  size_t i;
  double dx, dy, s;
  s = 0;
  for (i = 0; i < sizeof A / sizeof *A; i++) {
    dx = x - x0[i];
    dy = y - y00[i];
    s += A[i] * exp(a[i] * dx * dx + b[i] * dx * dy + c[i] * dy * dy);
  }
  return s;
}
static void F(double x, double y, double *u, double *v) {
  size_t i;
  double dx, dy, s;
  *u = 0;
  *v = 0;
  for (i = 0; i < sizeof A / sizeof *A; i++) {
    dx = x - x0[i];
    dy = y - y00[i];
    s = A[i] * exp(a[i] * dx * dx + b[i] * dx * dy + c[i] * dy * dy);
    *u += (2 * a[i] * dx + b[i] * dy) * s;
    *v += (b[i] * dx + 2 * c[i] * dy) * s;
  }
}
