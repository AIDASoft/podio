#!/usr/bin/env python3
"""Python module for defining the basic reader interface that is used by the
backend specific bindings"""


from podio.frame_iterator import FrameCategoryIterator


class BaseReaderMixin:
  """Mixin class the defines the base interface of the readers.

  The backend specific readers inherit from here and have to initialize the
  following members:
  - _reader: The actual reader that is able to read frames
  """

  def __init__(self):
    """Initialize common members.

    In inheriting classes this needs to be called **after** the _reader has been
    setup.
    """
    self._categories = tuple(s.data() for s in self._reader.getAvailableCategories())
    if hasattr(self, '_is_legacy'):
      self._is_legacy = getattr(self, '_is_legacy')
    else:
      self._is_legacy = False  # by default assume we are not legacy

  @property
  def categories(self):
    """Get the available categories from this reader.

    Returns:
        tuple(str): The names of the available categories from this reader
    """
    return self._categories

  def get(self, category):
    """Get an iterator with access functionality for a given category.

    Args:
        category (str): The name of the desired category

    Returns:
        FrameCategoryIterator: The iterator granting access to all Frames of the
            desired category
    """
    return FrameCategoryIterator(self._reader, category)

  @property
  def is_legacy(self):
    """Whether this is a legacy file reader or not.

    Returns:
        bool: True if this is a legacy file reader
    """
    return self._is_legacy

  @property
  def edm_definitions(self):
    """Get the available EDM definitions from this reader.

    Returns:
        tuple(str): The names of the available EDM definitions
    """
    if self._is_legacy:
      return ()
    return tuple(n.c_str() for n in self._reader.getAvailableEDMDefinitions())

  def get_edm_definition(self, edm_name):
    """Get the EDM definition of the passed EDM as JSON string.

    Args:
        str: The name of the EDM

    Returns:
        str: The complete model definition in JSON format. Use, e.g. json.loads
            to convert it into a python dictionary.
    """
    if self._is_legacy:
      return ""
    return self._reader.getEDMDefinition(edm_name).data()
