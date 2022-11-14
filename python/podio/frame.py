#!/usr/bin/env python3
"""Module for the python bindings of the podio::Frame"""

# pylint: disable-next=import-error # gbl is a dynamic module from cppyy
from cppyy.gbl import std

import ROOT
# NOTE: It is necessary that this can be found on the ROOT_INCLUDE_PATH
ROOT.gInterpreter.LoadFile('podio/Frame.h')  # noqa: E402
from ROOT import podio  # noqa: E402 # pylint: disable=wrong-import-position


def _determine_supported_parameter_types(lang):
  """Determine the supported types for the parameters.

  Args:
      lang (str): Language for which the type names should be returned. Either
          'c++' or 'py'.

  Returns:
      tuple (str): the tuple with the string representation of all **c++**
          classes that are supported
  """
  types_tuple = podio.SupportedGenericDataTypes()
  n_types = std.tuple_size[podio.SupportedGenericDataTypes].value

  # Get the python types with the help of cppyy and the STL
  py_types = (type(std.get[i](types_tuple)).__name__ for i in range(n_types))
  if lang == 'py':
    return tuple(py_types)
  if lang == 'c++':
    # Map of types that need special care when going from python to c++
    py_to_cpp_type_map = {
        'str': 'std::string'
        }
    # Convert them to the corresponding c++ types
    return tuple(py_to_cpp_type_map.get(t, t) for t in py_types)

  raise ValueError(f"lang needs to be 'py' or 'c++' (got {lang})")


SUPPORTED_PARAMETER_TYPES = _determine_supported_parameter_types('c++')
SUPPORTED_PARAMETER_PY_TYPES = _determine_supported_parameter_types('py')


# Map that is necessary for easier disambiguation of parameters that are
# available with more than one type under the same name. Maps a python type to
# a c++ vector of the corresponding type or a c++ type to the vector
_PY_TO_CPP_TYPE_MAP = {
    pytype: f'std::vector<{cpptype}>' for (pytype, cpptype) in zip(SUPPORTED_PARAMETER_PY_TYPES,
                                                                     SUPPORTED_PARAMETER_TYPES)
    }
_PY_TO_CPP_TYPE_MAP.update({
    f'{cpptype}': f'std::vector<{cpptype}>' for cpptype in SUPPORTED_PARAMETER_TYPES
    })


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

    self._collections = tuple(str(s) for s in self._frame.getAvailableCollections())
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

  def get_parameter(self, name, as_type=None):
    """Get the parameter stored under the given name.

    Args:
        name (str): The name of the parameter
        as_type (str, optional): Type specifier to disambiguate between
            parameters with the same name but different types. If there is only
            one parameter with a given name, this argument is ignored

    Returns:
        int, float, str or list of those: The value of the stored parameter

    Raises:
        KeyError: If no parameter is stored under the given name
        ValueError: If there are multiple parameters with the same name, but
            multiple types and no type specifier to disambiguate between them
            has been passed.

    """
    def _get_param_value(par_type, name):
      par_value = self._frame.getParameter[par_type](name)
      if len(par_value) > 1:
        return list(par_value)
      return par_value[0]

    # This access already raises the KeyError if there is no such parameter
    par_type = self._param_key_types[name]
    # Exactly one parameter, nothing more to do here
    if len(par_type) == 1:
      return _get_param_value(par_type[0], name)

    if as_type is None:
      raise ValueError(f'{name} parameter has {len(par_type)} different types available, '
                       'but no as_type argument to disambiguate')

    req_type = _PY_TO_CPP_TYPE_MAP.get(as_type, None)
    if req_type is None:
      raise ValueError(f'as_type value {as_type} cannot be mapped to a valid parameter type')

    if req_type not in par_type:
      raise ValueError(f'{name} parameter is not available as type {as_type}')

    return _get_param_value(req_type, name)

  def get_parameters(self):
    """Get the complete podio::GenericParameters object stored in this Frame.

    NOTE: This is mainly intended for dumping things, for actually obtaining
    parameters please use get_parameter

    Returns:
        podio.GenericParameters: The stored generic parameters
    """
    # Going via the not entirely inteded way here
    return self._frame.getGenericParametersForWrite()

  def get_param_info(self, name):
    """Get the parameter type information stored under the given name.

    Args:
        name (str): The parameter name

    Returns:
        dict (str: int): The c++-type(s) of the stored parameter and the number of
            parameters

    Raise:
        KeyError: If no parameter is stored under the given name
    """
    # This raises the KeyError if the name is not present
    par_types = [t.replace('std::vector<', '').replace('>', '') for t in self._param_key_types[name]]
    # Assume that we have one parameter and update the dictionary below in case
    # there are more
    par_infos = {t: 1 for t in par_types}
    for par_type in par_types:
      par_value = self.get_parameter(name, as_type=par_type)
      if isinstance(par_value, list):
        par_infos[par_type] = len(par_value)

    return par_infos

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
      for key in keys:
        # Make sure to convert to a python string here to not have a dangling
        # reference here for the key.
        key = str(key)
        # In order to support the use case of having the same key for multiple
        # types create a list of available types for the key, so that we can
        # disambiguate later. Storing a vector<type> here, and check later how
        # many elements there actually are to decide whether to return a single
        # value or a list
        if key not in keys_dict:
          keys_dict[key] = [f'std::vector<{par_type}>']
        else:
          keys_dict[key].append(f'std::vector<{par_type}>')

    return keys_dict
