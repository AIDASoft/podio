#!/usr/bin/env python3
"""Python module for reading sio files containing podio Frames"""

from podio.base_reader import BaseReaderMixin

from ROOT import gSystem
gSystem.Load('libpodioSioIO')  # noqa: 402
from ROOT import podio  # noqa: 402 # pylint: disable=wrong-import-position

Writer = podio.SIOFrameWriter


class Reader(BaseReaderMixin):
  """Reader class for readion podio SIO files."""

  def __init__(self, filename):
    """Create a reader that reads from the passed file.

    Args:
        filename (str): File to open and read data from
    """
    self._reader = podio.SIOFrameReader()
    self._reader.openFile(filename)

    super().__init__()


class LegacyReader(BaseReaderMixin):
  """Reader class for reading legcy podio sio files.

  This reader can be used to read files that have not yet been written using the
  Frame based I/O into Frames for a more seamless transition.
  """

  def __init__(self, filename):
    """Create a reader that reads from the passed file.

    Args:
        filename (str): File to open and read data from
    """
    self._reader = podio.SIOLegacyReader()
    self._reader.openFile(filename)
    self._is_legacy = True

    super().__init__()
