"""Python module for the podio EDM toolkit and its utilities"""
import sys

from .__version__ import __version__

from .podio_config_reader import *  # noqa: F403, F401

# Try to load podio, this is equivalent to trying to load libpodio.so and will
# error if libpodio.so is not found but work if it's found
try:
  from ROOT import podio
except ImportError:
  print('Unable to load podio, make sure that libpodio.so is in LD_LIBRARY_PATH')
else:
  from .frame import Frame
  from . import root_io, reading

  try:
    # We try to import the sio bindings which may fail if ROOT is not able to
    # load the dictionary. In this case they have most likely not been built and
    # we just move on
    from . import sio_io
  except ImportError:
    pass

  from . import EventStore

  try:
    # For some reason the test_utils only work at (test) runtime if they are
    # imported with the rest of podio. Otherwise they produce a weird c++ error.
    # This happens even if we import the *exact* same file.
    from . import test_utils  # noqa: F401
  except ImportError:
    pass

  # Make sure that this module is actually usable as podio even though most of
  # it is dynamically populated by cppyy
  sys.modules["podio"] = podio

  __all__ = [
      "__version__",
      "Frame",
      "root_io",
      "sio_io",
      "reading",
      "EventStore"
      ]
