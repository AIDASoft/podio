"""Python module for the podio EDM toolkit and its utilities"""

from .__version__ import __version__

# Try to load podio, this is equivalent to trying to load libpodio.so and will
# error if libpodio.so is not found but work if it's found
try:
    from ROOT import podio  # noqa: F401
except ImportError:
    print("Unable to load podio, make sure that libpodio is in LD_LIBRARY_PATH")
    raise

from .frame import Frame
from .link_navigator import LinkNavigator, ReturnFrom, ReturnTo, LinkCollection
from . import root_io, reading, version

try:
    # We try to import the sio bindings which may fail if ROOT is not able to
    # load the dictionary. In this case they have most likely not been built and
    # we just move on
    from . import sio_io
except ImportError:
    pass

try:
    # Same mechanism as for the sio_io above
    from . import data_source
except ImportError:
    pass

__all__ = [
    "__version__",
    "Frame",
    "LinkNavigator",
    "LinkCollection",
    "ReturnFrom",
    "ReturnTo",
    "root_io",
    "sio_io",
    "reading",
    "data_source",
    "version",
]
