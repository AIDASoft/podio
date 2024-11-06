#!/usr/bin/env python3
"""Python module for reading sio files containing podio Frames"""

from ROOT import gSystem
from root_io import convert_to_str_paths

if gSystem.DynamicPathName("libpodioSioIO.so", True):
    gSystem.Load("libpodioSioIO")  # noqa: 402
else:
    raise ImportError("Error when importing libpodioSioIO")
from ROOT import podio  # noqa: 402 # pylint: disable=wrong-import-position

from podio.base_reader import BaseReaderMixin  # pylint: disable=wrong-import-position
from podio.base_writer import BaseWriterMixin  # pylint: disable=wrong-import-position


class Reader(BaseReaderMixin):
    """Reader class for reading podio SIO files."""

    def __init__(self, filename):
        """Create a reader that reads from the passed file.

        Args:
            filename (str or Path): File to open and read data from.
        """
        filename = convert_to_str_paths(filename)[0]
        self._reader = podio.SIOReader()
        self._reader.openFile(filename)

        super().__init__()


class LegacyReader(BaseReaderMixin):
    """Reader class for reading legacy podio sio files.

    This reader can be used to read files that have not yet been written using the
    Frame based I/O into Frames for a more seamless transition.
    """

    def __init__(self, filename):
        """Create a reader that reads from the passed file.

        Args:
            filename (str or Path): File to open and read data from.
        """
        filename = convert_to_str_paths(filename)[0]
        self._reader = podio.SIOLegacyReader()
        self._reader.openFile(filename)
        self._is_legacy = True

        super().__init__()


class Writer(BaseWriterMixin):
    """Writer class for writing podio root files."""

    def __init__(self, filename):
        """Create a writer for writing files.

        Args:
            filename (str or Path): The name of the output file.
        """
        filename = convert_to_str_paths(filename)[0]
        self._writer = podio.SIOWriter(filename)

        super().__init__()
