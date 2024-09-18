"""Python module for creating ROOT RDataFrame with files containing podio Frames"""

from ROOT import gSystem

if (
    not gSystem.DynamicPathName("libpodioDataSourceDict", True)
    or gSystem.Load("libpodioDataSourceDict") < 0
):
    raise ImportError("Error when loading libpodioDataSourceDict")

from ROOT import podio  # pylint: disable=wrong-import-position

CreateDataFrame = podio.CreateDataFrame
