#!/usr/bin/env python3
"""Python module for defining the basic writer interface that is used by the
backend specific bindings"""

import atexit


class AllWriters:
    """Class to manage all writers in the program
    so that they can be properly finished at the end of the program
    """

    writers = []

    def add(self, writer):
        """Add a writer to the list of managed writers"""
        self.writers.append(writer)

    def finish(self):
        """Finish all managed writers"""
        for writer in self.writers:
            try:
                writer._writer.finish()  # pylint: disable=protected-access
            except AttributeError:
                pass


_all_writers = AllWriters()
atexit.register(_all_writers.finish)


class BaseWriterMixin:
    """Mixin class that defines the base interface of the writers.

    The backend specific writers inherit from here and have to initialize the
    following members:

    - _writer: The actual writer that is able to write frames
    """

    def __init__(self):
        """Initialize the writer"""
        _all_writers.add(self)

    def write_frame(self, frame, category, collections=None):
        """Write the given frame under the passed category, optionally limiting the
        collections that are written.

        Args:
            frame (podio.frame.Frame): The Frame to write
            category (str): The category name
            collections (optional, default=None): The subset of collections to
               write. If None, all collections are written
        """
        # pylint: disable=protected-access
        args = [frame._frame, category]
        if collections is not None:
            args.append(collections)
        self._writer.writeFrame(*args)
