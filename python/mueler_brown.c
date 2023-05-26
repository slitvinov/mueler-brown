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

struct Step {
  gsl_rng *rnd;
  double coeff;
};
static double E(double, double);
static void F(double, double, double *, double *);
static int step_ini(struct Step *);
static int step_fin(struct Step *);
static int step_next(struct Step *step, double *, double *, double *, double *);
static int state0(double, double);

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

static PyObject *state(PyObject *self, PyObject *args) {
  double x, y;
  if (!PyArg_ParseTuple(args, "dd", &x, &y))
    return NULL;
  return PyLong_FromLong(state0(x, y));
}

static PyObject *states(PyObject *self, PyObject *args) {
  int state, state0;
  long n, i, j, nbytes, period;
  double x, y, vx, vy, time;
  PyObject *buf;
  Py_buffer view;
  struct Step step;
  struct Transition {
    double time;
    int32_t state;
  } * out;
  if (!PyArg_ParseTuple(args, "lO", &n, &buf))
    return NULL;
  nbytes = n * sizeof(*out);
  if (!PyObject_CheckBuffer(buf))
    return NULL;
  if (PyObject_GetBuffer(buf, &view, PyBUF_SIMPLE | PyBUF_WRITABLE) == -1)
    return NULL;
  if (view.len < nbytes) {
    PyErr_SetString(PyExc_ValueError, "size of the buffer is too small");
    return NULL;
  }
  out = view.buf;
  x = Param.x0;
  y = Param.y0;
  vx = 0;
  vy = 0;
  time = 0;
  step_ini(&step);
  period = 100;
  state0 = -1;
  if (step_ini(&step) != 0) {
    PyErr_SetString(PyExc_ValueError, "step_ini failed");
    return NULL;
  }
  j = 0;
  for (i = 0; i < n;) {
    if (j == 0 || j == period) {
      if (PyErr_CheckSignals() != 0)
        break;
      j = 0;
    }
    j++;
    // (−0.57, 1.45), (0.15, 0.3),
    // (0.45, 0.05), (0.35, 0.15),
    state = x > 0;

    if (state != state0) {
      state0 = state;
      out[i].time = time;
      out[i].state = state;
      i++;
      if (i == n)
        break;
    }
    step_next(&step, &x, &y, &vx, &vy);
    time += Param.dt;
  }
  step_fin(&step);
  PyBuffer_Release(&view);
  Py_RETURN_NONE;
}

static PyObject *trajectory(PyObject *self, PyObject *args) {
  long n, i, j, k, period, nbytes;
  double x, y, vx, vy, *out;
  PyObject *buf;
  Py_buffer view;
  struct Step step;
  if (!PyArg_ParseTuple(args, "llO", &n, &period, &buf))
    return NULL;
  nbytes = 2 * n * sizeof(double);
  if (!PyObject_CheckBuffer(buf))
    return NULL;
  if (PyObject_GetBuffer(buf, &view, PyBUF_SIMPLE | PyBUF_WRITABLE) == -1)
    return NULL;
  if (view.len < nbytes) {
    PyErr_SetString(PyExc_ValueError, "size of the buffer is too small");
    return NULL;
  }
  out = view.buf;
  x = Param.x0;
  y = Param.y0;
  vx = 0;
  vy = 0;
  step_ini(&step);
  j = 0;
  k = 0;
  if (step_ini(&step) != 0) {
    PyErr_SetString(PyExc_ValueError, "step_ini failed");
    return NULL;
  }
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
    step_next(&step, &x, &y, &vx, &vy);
  }
  step_fin(&step);
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
    {"state", state, METH_VARARGS, PyDoc_STR("Returns the state")},
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
static int step_ini(struct Step *step) {
  gsl_rng_env_setup();
  if ((step->rnd = gsl_rng_alloc(gsl_rng_default)) == NULL)
    return 1;
  step->coeff = sqrt(2 * Param.kT * Param.gamma * Param.dt);
  return 0;
}
static int step_fin(struct Step *step) {
  gsl_rng_free(step->rnd);
  return 0;
}
static int step_next(struct Step *step, double *x, double *y, double *vx,
                     double *vy) {
  double fx, fy, rx, ry;
  F(*x, *y, &fx, &fy);
  rx = gsl_ran_gaussian(step->rnd, 1);
  ry = gsl_ran_gaussian(step->rnd, 1);
  *vx += Param.dt * (-fx - Param.gamma * *vx) + step->coeff * rx;
  *vy += Param.dt * (-fy - Param.gamma * *vy) + step->coeff * ry;
  *x += Param.dt * *vx;
  *y += Param.dt * *vy;
  return 0;
}
static int state0(double u, double v) {
  const double pi = 3.141592653589793;
  double x, y;

  x = u - (-0.57);
  y = v - 1.45;
  x /= 0.15;
  y /= 0.3;

  if (x * x + y * y < 1)
    return 0;

  x = u - 0.45;
  y = v - 0.05;

  u = x * cos(pi / 4) - y * sin(pi / 4);
  v = x * sin(pi / 4) + y * cos(pi / 4);

  u /= 0.35;
  v /= 0.15;

  return u * u + v * v < 1 ? 1 : 2;
}
