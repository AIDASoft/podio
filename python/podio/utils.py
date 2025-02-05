#!/usr/bin/env python3
"""utility functionality for podio"""

import os
from collections.abc import Iterable
from pathlib import Path

import ROOT

if ROOT.gInterpreter.LoadFile("podio/podioVersion.h") != 0:  # noqa: E402
    raise ImportError("Cannot find the podio/podioVersion.h header")


def convert_to_str_paths(filenames):
    """Converts filenames to string paths, handling both string and pathlib.Path objects and
       iterables of such objects.

    Args:
        filenames (str, Path, or Iterable[str | Path]): A single filepath or an iterable of
        filepaths to convert to str object(s).

    Returns:
        list[str]: A list of filepaths as strings.
    """

    if isinstance(filenames, Iterable) and not isinstance(filenames, (str, Path)):
        return [os.fspath(fn) for fn in filenames]

    return [os.fspath(filenames)]


def expand_glob(pattern):
    """
    Expands a given glob pattern into a list of matching file paths.

    This function takes a glob pattern as input and returns a list of strings
    containing the paths that match the pattern. It supports standard glob rules
    extended with tilde expansion and brace expansion. If the pattern doesn't
    contain any wildcards, it is placed in the returned list as is. Paths that
    cannot be accessed are displayed on stderr, but the expansion process is not
    aborted.

    Args:
        pattern (str): The glob pattern to expand.

    Returns:
        list of str: A list of strings containing the matching file paths.

    Raises:
        cppyy.gbl.std.runtime_error: If no matches are found or if there is an error during glob
        expansion.
    """
    return [str(x) for x in ROOT.podio.utils.expand_glob(pattern)]


is_glob_pattern = ROOT.podio.utils.is_glob_pattern
