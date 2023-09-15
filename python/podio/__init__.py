"""Python module for the podio EDM toolkit and its utilities"""
import sys

from .__version__ import __version__

from .podio_config_reader import *  # noqa: F403, F401

import ROOT

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
  from . import root_io, sio_io, reading

  from . import EventStore

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
