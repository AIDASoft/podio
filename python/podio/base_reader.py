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
