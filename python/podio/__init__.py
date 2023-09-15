"""Python module for the podio EDM toolkit and its utilities"""
import sys

from .__version__ import __version__

import ROOT

if ROOT.gSystem.Load("libpodioDict.so") < 0:
    raise RuntimeError("Failed to load libpodioDict.so")

from ROOT import podio

from .podio_config_reader import *

from .frame import Frame

from . import root_io, sio_io, reading

from .test_utils import *

from . import EventStore

sys.modules["podio"] = podio
