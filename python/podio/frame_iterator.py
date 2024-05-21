#!/usr/bin/env python3
"""Module defining the Frame iterator used by the Reader interface"""

# pylint: disable-next=import-error # gbl is a dynamic module from cppyy
from cppyy.gbl import std
from podio.frame import Frame


class FrameCategoryIterator:
    """Iterator for iterating over all Frames of a given category available from a
    reader as well as accessing specific entries
    """

    def __init__(self, reader, category):
        """Construct the iterator from the reader and the category.

        Args:
            reader (Reader): Any podio reader offering access to Frames
            category (str): The category name of the Frames to be iterated over
        """
        self._reader = reader
        self._category = category

    def __iter__(self):
        """The trivial implementation for the iterator protocol."""
        return self

    def __next__(self):
        """Get the next available Frame or stop."""
        frame_data = self._reader.readNextEntry(self._category)
        if frame_data:
            return Frame(std.move(frame_data))

        raise StopIteration

    def __len__(self):
        """Get the number of available Frames for the passed category."""
        return self._reader.getEntries(self._category)

    def __getitem__(self, entry):
        """Get a specific entry.

        Args:
            entry (int): The entry to access
        """
        # Handle python negative indexing to start from the end
        if entry < 0:
            entry = self._reader.getEntries(self._category) + entry

        if entry < 0:
            # If we are below 0 now, we do not have enough entries to serve the request
            raise IndexError

        try:
            frame_data = self._reader.readEntry(self._category, entry)
        except std.bad_function_call:
            print(
                "Error: Unable to read an entry of the input file. This can "
                "happen when the ROOT model dictionaries are not in "
                "LD_LIBRARY_PATH. Make sure that LD_LIBRARY_PATH points to the "
                "library folder of the installation of podio and also to the"
                "library folder with your data model\n"
            )
            raise

        if frame_data:
            return Frame(std.move(frame_data))

        raise IndexError
