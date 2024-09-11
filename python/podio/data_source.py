"""Python module for creating ROOT RDataFrame with files containing podio Frames"""

from ROOT import gSystem, gInterpreter

if not gSystem.DynamicPathName("libpodioDataSource", True):
    raise ImportError("Error finding libpodioDataSource")

if gInterpreter.LoadFile("podio/DataSource.h") != 0:
    raise ImportError("Error when loading file podio/DataSource.h")

from ROOT import podio  # pylint: disable=wrong-import-position

CreateDataFrame = podio.CreateDataFrame
