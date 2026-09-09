#!/usr/bin/env python3
"""Python module for reading arrow files containing podio Frames"""

from ROOT import gSystem

if gSystem.DynamicPathName("libpodioArrow.so", True):
    gSystem.Load("libpodioArrow")  # noqa: E402
else:
    raise ImportError("Error when importing libpodioArrow")
from ROOT import podio  # noqa: E402 # pylint: disable=wrong-import-position

from podio.base_reader import BaseReaderMixin  # pylint: disable=wrong-import-position
from podio.base_writer import BaseWriterMixin  # pylint: disable=wrong-import-position
from podio.utils import convert_to_str_paths  # pylint: disable=wrong-import-position # noqa: E402


class Reader(BaseReaderMixin):
    """Reader class for reading podio Arrow files."""

    def __init__(self, directory):
        """Create a reader that reads from the passed directory.

        Args:
            directory (str or Path): Directory to open and read data from.
        """
        directory = convert_to_str_paths(directory)[0]
        self._reader = podio.ArrowReader()
        self._reader.openFile(directory)

        super().__init__()


class Writer(BaseWriterMixin):
    """Writer class for writing podio arrow files."""

    def __init__(self, directory):
        """Create a writer for writing files.

        Args:
            directory (str or Path): The name of the output directory.
        """
        directory = convert_to_str_paths(directory)[0]
        self._writer = podio.ArrowWriter(directory)

        super().__init__()
