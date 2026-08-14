#!/usr/bin/env python3
"""Python module for reading arrow files containing podio Frames"""

from ROOT import gSystem

if gSystem.DynamicPathName("libpodioArrow.so", True):
    gSystem.Load("libpodioArrow")  # noqa: 402
else:
    raise ImportError("Error when importing libpodioArrow")
from ROOT import podio  # noqa: 402 # pylint: disable=wrong-import-position

from podio.base_reader import BaseReaderMixin  # pylint: disable=wrong-import-position
from podio.base_writer import BaseWriterMixin  # pylint: disable=wrong-import-position
from podio.utils import convert_to_str_paths  # pylint: disable=wrong-import-position # noqa: E402


class Reader(BaseReaderMixin):
    """Reader class for reading podio Arrow files."""

    def __init__(self, filename):
        """Create a reader that reads from the passed directory.

        Args:
            filename (str or Path): Directory to open and read data from.
        """
        filename = convert_to_str_paths(filename)[0]
        self._reader = podio.ArrowReader()
        self._reader.openFile(filename)

        super().__init__()


class Writer(BaseWriterMixin):
    """Writer class for writing podio arrow files."""

    def __init__(self, filename):
        """Create a writer for writing files.

        Args:
            filename (str or Path): The name of the output directory.
        """
        filename = convert_to_str_paths(filename)[0]
        self._writer = podio.ArrowWriter(filename)

        super().__init__()
