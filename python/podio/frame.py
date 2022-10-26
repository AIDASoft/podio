#!/usr/bin/env python3
"""Module for the python bindings of the podio::Frame"""

# pylint: disable-next=import-error # gbl is a dynamic module from cppyy
from cppyy.gbl import std

import ROOT
# NOTE: It is necessary that this can be found on the ROOT_INCLUDE_PATH
ROOT.gInterpreter.LoadFile('podio/Frame.h')  # noqa: E402
from ROOT import podio  # noqa: E402 # pylint: disable=wrong-import-position


def _determine_supported_parameter_types():
  """Determine the supported types for the parameters.

  Returns:
      tuple (str): the tuple with the string representation of all **c++**
        classes that are supported
  """
  types_tuple = podio.SupportedGenericDataTypes()
  n_types = std.tuple_size[podio.SupportedGenericDataTypes].value

  # Map of types that need special care when going from python to c++
  py_to_cpp_type_map = {
      'str': 'std::string'
      }
  # Get the python types with the help of cppyy and the STL
  py_types = (type(std.get[i](types_tuple)).__name__ for i in range(n_types))
  # Convert them to the corresponding c++ types
  return tuple(py_to_cpp_type_map.get(t, t) for t in py_types)


SUPPORTED_PARAMETER_TYPES = _determine_supported_parameter_types()


class Frame:
  """Frame class that serves as a container of collection and meta data."""

  def __init__(self, data=None):
    """Create a Frame.

    Args:
        data (FrameData, optional): Almost arbitrary FrameData, e.g. from file
    """
    # Explicitly check for None here, to not return empty Frames on nullptr data
    if data is not None:
      self._frame = podio.Frame(data)
    else:
      self._frame = podio.Frame()

    self._collections = tuple(s for s in self._frame.getAvailableCollections())
    self._param_key_types = self._init_param_keys()

  @property
  def collections(self):
    """Get the available collection (names) from this Frame.

    Returns:
        tuple(str): The names of the available collections from this Frame.
    """
    return self._collections

  def get(self, name):
    """Get a collection from the Frame by name.

    Args:
        name (str): The name of the desired collection

    Returns:
        collection (podio.CollectionBase): The collection stored in the Frame

    Raises:
        KeyError: If the collection with the name is not available
    """
    collection = self._frame.get(name)
    if not collection:
      raise KeyError
    return collection

  @property
  def parameters(self):
    """Get the available parameter names from this Frame.

    Returns:
        tuple (str): The names of the available parameters from this Frame.
    """
    return tuple(self._param_key_types.keys())

  def get_parameter(self, name):
    """Get the parameter stored under the given name.

    Args:
        name (str): The name of the parameter

    Returns:
        int, float, str or list of those: The value of the stored parameter

    Raises:
        KeyError: If no parameter is stored under the given name
    """
    # This access already raises the KeyError if there is no such parameter
    par_type = self._param_key_types[name]
    par_value = self._frame.getParameter[par_type](name)
    if len(par_value) > 1:
      return list(par_value)
    return par_value[0]

  def _init_param_keys(self):
    """Initialize the param keys dict for easier lookup of the available parameters.

    NOTE: This depends on a "side channel" that is usually reserved for the
    writers but is currently still in the public interface of the Frame

    Returns:
        dict: A dictionary mapping each key to the corresponding c++ type
    """
    params = self._frame.getGenericParametersForWrite()  # this is the iffy bit
    keys_dict = {}
    for par_type in SUPPORTED_PARAMETER_TYPES:
      keys = params.getKeys[par_type]()
      # Make sure to convert to a python string here to not have a dangling
      # referenc here for the key. Also make the type an std::vector so that we
      # always call the getter that obtains the full vector and we decide later
      # in the python layer whether we return a single value or a list of values
      keys_dict.update({str(k): f'std::vector<{par_type}>' for k in keys})

    return keys_dict
