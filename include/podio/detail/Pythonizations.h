#ifndef PODIO_DETAIL_PYTHONIZATIONS_H
#define PODIO_DETAIL_PYTHONIZATIONS_H

#include <Python.h>
#include <stdexcept>
#include <string>

namespace podio::detail::pythonizations {

static PyObject* subscript(PyObject* self, PyObject* index) {
  PyObject* result = PyObject_CallMethod(self, "at", "O", index);
  if (!result && PyErr_Occurred()) { // TODO: set IndexError only if cppyy.gbl.std.out_of_bounds occurred
    PyErr_SetString(PyExc_IndexError, "Index out of range");
  }
  return result;
}

static void pythonize_subscript(PyObject* klass, const std::string& name) {
  static PyMethodDef ml = {"subscipt_pythonization", subscript, METH_VARARGS, R"(
        Raise an `IndexError` exception if an index is invalid.
        The `__getitem__` will return immutable datatype objects instead of the mutable ones.
  )"};
  auto* func = PyCFunction_New(&ml, klass);
  if (!func) {
    throw std::runtime_error("Failed to create Python subscript function for class " + name);
  }
  auto* method = PyInstanceMethod_New(func);
  if (!method) {
    Py_DECREF(func);
    throw std::runtime_error("Failed to create Python instance method for subscript for class " + name);
  }
  if (0 != PyObject_SetAttrString(klass, "__getitem__", method)) {
    Py_DECREF(method);
    Py_DECREF(func);
    throw std::runtime_error("Failed to set __getitem__ attribute on class " + name);
  }
  Py_DECREF(func);
  Py_DECREF(method);
}

} // namespace podio::detail::pythonizations

#endif // PODIO_DETAIL_PYTHONIZATIONS_H
