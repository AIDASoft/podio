import os
from collections.abc import Iterable
from pathlib import Path


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
