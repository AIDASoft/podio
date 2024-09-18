#!/usr/bin/env python3
"""Module that facilitates working with the podio::version::Version"""

import ROOT

# NOTE: It is necessary that this can be found on the ROOT_INCLUDE_PATH
#
# We check whether we can actually load the header to not break python bindings
# in environments with *ancient* podio versions
if ROOT.gInterpreter.LoadFile("podio/podioVersion.h") == 0:  # noqa: E402
    from ROOT import podio  # noqa: E402 # pylint: disable=wrong-import-position

Version = podio.version.Version


def _str_dunder(self):
    """Shim to get a more reasonable string representation"""
    return f"{self.major}.{self.minor}.{self.patch}"


Version.__str__ = _str_dunder


def parse(*args):
    """Construct a version from either a list of integers or a version string"""
    if len(args) == 1:
        if isinstance(args[0], podio.version.Version):
            return args[0]
        if isinstance(args[0], str):
            ver_tuple = tuple(int(v) for v in args[0].split("."))
        else:
            ver_tuple = (int(args[0]),)
    else:
        ver_tuple = tuple(args)
    ver = Version()
    for mem, val in zip(("major", "minor", "patch"), ver_tuple):
        setattr(ver, mem, val)
    return ver


# The version with which podio has been built. Same as __version__
build_version = podio.version.build_version
