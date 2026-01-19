#ifndef PODIO_DETAIL_PYTHONIZATIONS_H
#define PODIO_DETAIL_PYTHONIZATIONS_H

#include <string>

typedef struct _object PyObject;

namespace podio::detail::pythonizations {

// Callback function for the subscript pythonization
// Calls the `at` method and change exception type to IndexError if the index is out of range
PyObject* subscript(PyObject* self, PyObject* index);

// Helper to register the subscript pythonization callback as `__getitem__` method
void pythonize_subscript(PyObject* klass, const std::string& name);

} // namespace podio::detail::pythonizations

#endif // PODIO_DETAIL_PYTHONIZATIONS_H
