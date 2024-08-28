#!/usr/bin/env python3
"""Module that facilitates working with the podio::version::Version"""

import ROOT

# NOTE: It is necessary that this can be found on the ROOT_INCLUDE_PATH
#
# We check whether we can actually load the header to not break python bindings
# in environments with *ancient* podio versions
if ROOT.gInterpreter.LoadFile("podio/podioVersion.h") == 0:  # noqa: E402
    from ROOT import podio  # noqa: E402 # pylint: disable=wrong-import-position

build_version = podio.version.build_version


def version_as_str(ver):
    """Stringify the version into the usual format

    Args:
        ver (podio.version.Version): A podio version

    Returns:
        str: A stringified version of the version, in the format
            MAJOR.MINOR.PATCH
    """
    return f"{ver.major}.{ver.minor}.{ver.patch}"
