#ifndef PODIO_DETAIL_PYTHONIZATIONS_H
#define PODIO_DETAIL_PYTHONIZATIONS_H

// NOTE: Python.h must be included before any standard headers and should be preceded by PY_SSIZE_T_CLEAN definition
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <cstring>
#include <stdexcept>
#include <string>

namespace podio::detail::pythonizations {

// Callback function for the subscript pythonization
// Calls the `at` method and change exception type to IndexError if the index is out of range
inline PyObject* subscript(PyObject* self, PyObject* index) {
  PyObject* result = PyObject_CallMethod(self, "at", "O", index);
  if (!result) {
    PyObject* exc = PyErr_Occurred();
    // Check if the exception is `cppyy.gbl.std.out_of_range`
    // Since PyImport_ImportModule("cppyy") fails, this workaround checks the exception name
    if (exc && PyObject_HasAttrString(exc, "__name__")) {
      PyObject* exc_name = PyObject_GetAttrString(exc, "__name__");
      if (exc_name) {
        const char* name_cstr = PyUnicode_AsUTF8(exc_name);
        if (name_cstr && strcmp(name_cstr, "out_of_range") == 0) {
          PyErr_Clear();
          PyErr_SetString(PyExc_IndexError, "Index out of range");
        }
        Py_DECREF(exc_name);
      }
    }
  }
  return result;
}

// Helper to register the subscript pythonization callback as `__getitem__` method
inline void pythonize_subscript(PyObject* klass, const std::string& name) {
  static PyMethodDef ml = {"subscript_pythonization", subscript, METH_VARARGS, R"(
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
