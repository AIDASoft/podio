#!/usr/bin/env python3
"""Python module for reading root files containing podio Frames"""

from ROOT import gSystem
gSystem.Load('libpodioRootIO')  # noqa: E402
from ROOT import podio  # noqa: E402 # pylint: disable=wrong-import-position

from podio.base_reader import BaseReaderMixin  # pylint: disable=wrong-import-position
from podio.base_writer import BaseWriterMixin  # pylint: disable=wrong-import-position


class Reader(BaseReaderMixin):
  """Reader class for reading podio root files."""

  def __init__(self, filenames):
    """Create a reader that reads from the passed file(s).

    Args:
        filenames (str or list[str]): file(s) to open and read data from
    """
    if isinstance(filenames, str):
      filenames = (filenames,)

    self._reader = podio.ROOTFrameReader()
    self._reader.openFiles(filenames)

    super().__init__()


class RNTupleReader(BaseReaderMixin):
  """Reader class for reading podio RNTuple root files."""

  def __init__(self, filenames):
    """Create an RNTuple reader that reads from the passed file(s).

    Args:
        filenames (str or list[str]): file(s) to open and read data from
    """
    if isinstance(filenames, str):
      filenames = (filenames,)

    self._reader = podio.ROOTRNTupleReader()
    self._reader.openFiles(filenames)

    super().__init__()


class LegacyReader(BaseReaderMixin):
  """Reader class for reading legacy podio root files.

  This reader can be used to read files that have not yet been written using
  Frame based I/O into Frames for a more seamless transition.
  """

  def __init__(self, filenames):
    """Create a reader that reads from the passed file(s).

    Args:
        filenames (str or list[str]): file(s) to open and read data from
    """
    if isinstance(filenames, str):
      filenames = (filenames,)

    self._reader = podio.ROOTLegacyReader()
    self._reader.openFiles(filenames)
    self._is_legacy = True

    super().__init__()


class Writer(BaseWriterMixin):
  """Writer class for writing podio root files"""
  def __init__(self, filename):
    """Create a writer for writing files

    Args:
        filename (str): The name of the output file
    """
    self._writer = podio.ROOTFrameWriter(filename)
    super().__init__()


class RNTupleWriter(BaseWriterMixin):
  """Writer class for writing podio root files"""
  def __init__(self, filename):
    """Create a writer for writing files

    Args:
        filename (str): The name of the output file
    """
    self._writer = podio.ROOTRNTupleWriter(filename)
    super().__init__()
