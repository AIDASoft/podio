#!/usr/bin/env python3
"""Module that facilitates working with the podio::version::Version"""

from functools import total_ordering
from packaging import version

import ROOT

# NOTE: It is necessary that this can be found on the ROOT_INCLUDE_PATH
#
# We check whether we can actually load the header to not break python bindings
# in environments with *ancient* podio versions
if ROOT.gInterpreter.LoadFile("podio/podioVersion.h") == 0:  # noqa: E402
    from ROOT import podio  # noqa: E402 # pylint: disable=wrong-import-position


@total_ordering
class Version:
    """A dedicated version class for podio versions, as they are returned by
    the c++ interface.

    We make this so that version comparisons work as expected in python as well
    """

    def __init__(self, *args):
        """Construct a python version from its c++ counterpart

        Takes either a podio.version.Version, up to three numbers which are
        interpreted as major, minor, patch or a valid (python) version string
        """
        # We don't do a lot of validation here, we just convert everything into
        # a string and then let the version parsing fail in case the inputs are
        # wrong
        ver = args[0]
        if len(args) == 1:
            if isinstance(ver, podio.version.Version):
                ver = f"{ver.major}.{ver.minor}.{ver.patch}"
        else:
            ver = ".".join(str(v) for v in args)

        self.version = version.Version(ver)

    def __getattr__(self, attr):
        """Delegate attribute retrieval to the underlying version"""
        return getattr(self.version, attr)

    def __str__(self):
        return str(self.version)

    def __repr__(self):
        return f"podio.Version({self.version!r})"

    def __eq__(self, other):
        if isinstance(other, Version):
            return self.version == other.version
        if isinstance(other, version.Version):
            return self.version == other
        return NotImplemented

    def __lt__(self, other):
        if isinstance(other, Version):
            return self.version < other.version
        if isinstance(other, version.Version):
            return self.version < other
        return NotImplemented


# The version with which podio has been built. Same as __version__
build_version = Version(podio.version.build_version)
