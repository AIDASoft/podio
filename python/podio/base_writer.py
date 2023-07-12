#!/usr/bin/env python3
"""Python module for defining the basic writer interface that is used by the
backend specific bindings"""


class BaseWriterMixin:
  """Mixin class that defines the base interface of the writers.

  The backend specific writers inherit from here and have to initialize the
  following members:
    - _writer: The actual writer that is able to write frames
  """

  def write_frame(self, frame, category, collections=None):
    """Write the given frame under the passed category, optionally limiting the
    collections that are written.

    Args:
        frame (podio.frame.Frame): The Frame to write
        category (str): The category name
        collections (optional, default=None): The subset of collections to
           write. If None, all collections are written
    """
    self._writer.writeFrame(frame._frame, category, collections or list())
