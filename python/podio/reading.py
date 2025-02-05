#!/usr/bin/env python3
"""Module for general reading functionality."""

from ROOT import TFile

from podio import root_io

try:
    from podio import sio_io

    def _is_frame_sio_file(filename):
        """Peek into the sio file to determine whether this is a legacy file or not."""
        with open(filename, "rb") as sio_file:
            first_line = str(sio_file.readline())
            # The SIO Frame writer writes a podio_header_info at the beginning of the
            # file
            return first_line.find("podio_header_info") > 0

except ImportError:

    def _is_frame_sio_file(filename):
        """Stub raising a ValueError"""
        raise ValueError(
            "podio has not been built with SIO support, "
            "which is necessary to read this file, "
            "or there is a version mismatch"
        )


class RootFileFormat:
    """Enum to specify the ROOT file format"""

    TTREE = 0  # Non-legacy TTree based file
    RNTUPLE = 1  # RNTuple based file
    LEGACY = 2  # Legacy TTree based file


def _determine_root_format(filename):
    """Peek into the root file to determine which flavor we have at hand."""
    file = TFile.Open(filename)

    metadata = file.Get("podio_metadata")
    if not metadata:
        return RootFileFormat.LEGACY

    md_class = metadata.IsA().GetName()
    if "TTree" in md_class:
        return RootFileFormat.TTREE

    return RootFileFormat.RNTUPLE


def get_reader(filename):
    """Get an appropriate reader for the passed file.

    Args:
        filename (str): The input file

    Returns:
        root_io.[Legacy]Reader, sio_io.[Legacy]Reader: an initialized reader that
            is able to process the input file.

    Raises:
        ValueError: If the file cannot be recognized, or if podio has not been
            built with the necessary backend I/O support
    """
    if filename.endswith(".sio"):
        if _is_frame_sio_file(filename):
            return sio_io.Reader(filename)
        return sio_io.LegacyReader(filename)

    if filename.endswith(".root"):
        root_flavor = _determine_root_format(filename)
        if root_flavor == RootFileFormat.TTREE:
            return root_io.Reader(filename)
        if root_flavor == RootFileFormat.RNTUPLE:
            return root_io.RNTupleReader(filename)
        if root_flavor == RootFileFormat.LEGACY:
            return root_io.LegacyReader(filename)

    raise ValueError("file must end on .root or .sio")
