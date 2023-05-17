#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *E(PyObject *self, PyObject *args) {
  double x, y, eng;

  if (!PyArg_ParseTuple(args, "dd", &x, &y))
    return NULL;
  eng = x + y;
  return PyFloat_FromDouble(eng);
}

static PyMethodDef Mueler_BrownMethods[] = {
    {
        "E",
        E,
        METH_VARARGS,
        "Return energey",
    },
    {NULL, NULL, 0, NULL}, // sentinel
};

static PyModuleDef mueler_brownmodule = {
    PyModuleDef_HEAD_INIT, "mueler_brown", "Mueler-Brown potential", -1,
    Mueler_BrownMethods,
};

PyMODINIT_FUNC PyInit_mueler_brown(void) {
  PyObject *m;

  m = PyModule_Create(&mueler_brownmodule);
  return m;
}
