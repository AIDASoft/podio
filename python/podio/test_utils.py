#!/usr/bin/env python3
"""Utilities for python unittests"""

import os

SKIP_SIO_TESTS = os.environ.get("SKIP_SIO_TESTS", "1") == "1"


def get_legacy_input(filename):
    """Try to get a legacy input file by name from the ExternalData that is
    fetched by CMake.

    Returns either the absolute path to the actual file or an empty string.
    """
    try:
        datafile = os.path.join(os.environ["PODIO_BUILD_BASE"], "tests", "input_files", filename)
        if os.path.isfile(datafile):
            return os.path.abspath(datafile)
    except KeyError:
        pass
    return ""
