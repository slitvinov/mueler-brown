#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <math.h>

static struct {
  double kT, gamma, dt, x0, y0;
} Param = {.kT = 15,
           .gamma = 1,
           .dt = 0.01,
           .x0 = -5.5822363449547829e-01,
           .y0 = +1.4417258420221519e+00};

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

static PyObject *states(PyObject *self, PyObject *args) {
  long n, i, j, k, nbytes, period;
  double x, y, vx, vy, rx, ry, fx, fy, coeff, *out;
  gsl_rng *rnd;
  PyObject *buf;
  Py_buffer view;
  if (!PyArg_ParseTuple(args, "lO", &n, &buf))
    return NULL;
  nbytes = 2 * n * sizeof(double);
  if (!PyObject_CheckBuffer(buf))
    return NULL;
  if (PyObject_GetBuffer(buf, &view, PyBUF_SIMPLE | PyBUF_WRITABLE) == -1)
    return NULL;
  if (view.len < nbytes) {
    PyErr_SetString(PyExc_ValueError, "size of the buffe is too small");
    return NULL;
  }
  out = view.buf;
  gsl_rng_env_setup();
  if ((rnd = gsl_rng_alloc(gsl_rng_default)) == NULL) {
    PyErr_SetString(PyExc_ValueError, "gsl_rng_alloc failed");
    return NULL;
  }
  x = Param.x0;
  y = Param.y0;
  vx = 0;
  vy = 0;
  coeff = sqrt(2 * Param.kT * Param.gamma * Param.dt);
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
    vx += Param.dt * (-fx - Param.gamma * vx) + coeff * rx;
    vy += Param.dt * (-fy - Param.gamma * vy) + coeff * ry;
    x += Param.dt * vx;
    y += Param.dt * vy;
  }
  gsl_rng_free(rnd);
  PyBuffer_Release(&view);
  Py_RETURN_NONE;
}

static PyObject *trajectory(PyObject *self, PyObject *args) {
  long n, i, j, k, period, nbytes;
  double x, y, vx, vy, rx, ry, fx, fy, coeff, *out;
  gsl_rng *rnd;
  PyObject *buf;
  Py_buffer view;
  if (!PyArg_ParseTuple(args, "llO", &n, &period, &buf))
    return NULL;
  nbytes = 2 * n * sizeof(double);
  if (!PyObject_CheckBuffer(buf))
    return NULL;
  if (PyObject_GetBuffer(buf, &view, PyBUF_SIMPLE | PyBUF_WRITABLE) == -1)
    return NULL;
  if (view.len < nbytes) {
    PyErr_SetString(PyExc_ValueError, "size of the buffe is too small");
    return NULL;
  }
  out = view.buf;
  gsl_rng_env_setup();
  if ((rnd = gsl_rng_alloc(gsl_rng_default)) == NULL) {
    PyErr_SetString(PyExc_ValueError, "gsl_rng_alloc failed");
    return NULL;
  }
  x = Param.x0;
  y = Param.y0;
  vx = 0;
  vy = 0;
  coeff = sqrt(2 * Param.kT * Param.gamma * Param.dt);
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
    vx += Param.dt * (-fx - Param.gamma * vx) + coeff * rx;
    vy += Param.dt * (-fy - Param.gamma * vy) + coeff * ry;
    x += Param.dt * vx;
    y += Param.dt * vy;
  }
  gsl_rng_free(rnd);
  PyBuffer_Release(&view);
  Py_RETURN_NONE;
}

static PyMethodDef Mueler_BrownMethods[] = {
    {"energy", energy, METH_VARARGS, PyDoc_STR("Return energy")},
    {"force", force, METH_VARARGS, PyDoc_STR("Return force")},
    {"trajectory", trajectory, METH_VARARGS,
     PyDoc_STR("Returns paritcle trajectory")},
    {"states", states, METH_VARARGS,
     PyDoc_STR("Returns the sequence of the states")},
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
