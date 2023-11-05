"""Python module for the podio EDM toolkit and its utilities"""
import sys

from .__version__ import __version__

import ROOT  # pylint: disable=wrong-import-order

# Track whether we were able to dynamially load the library that is built by
# podio and enable certain features of the bindings only if they are actually
# available.
_DYNAMIC_LIBS_LOADED = False

# Check if we can locate the dictionary wthout loading it as this allows us to
# silence any ouptput. If we can find it, we can also safely load it
if ROOT.gSystem.DynamicPathName("libpodioDict.so", True):
  ROOT.gSystem.Load("libpodioDict.so")
  from ROOT import podio

  _DYNAMIC_LIBS_LOADED = True

if _DYNAMIC_LIBS_LOADED:
  from .frame import Frame
  from . import root_io, reading

  try:
    # We try to import the sio bindings which may fail if ROOT is not able to
    # load the dictionary in this case they have most likely not been built and
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
