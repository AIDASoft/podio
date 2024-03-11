"""Python module for the podio EDM toolkit and its utilities"""

from .__version__ import __version__

# Try to load podio, this is equivalent to trying to load libpodio.so and will
# error if libpodio.so is not found but work if it's found
try:
    from ROOT import podio  # noqa: F401
except ImportError:
    print("Unable to load podio, make sure that libpodio.so is in LD_LIBRARY_PATH")
    raise

from .frame import Frame
from . import root_io, reading

try:
    # We try to import the sio bindings which may fail if ROOT is not able to
    # load the dictionary. In this case they have most likely not been built and
    # we just move on
    from . import sio_io
except ImportError:
    pass

__all__ = [
    "__version__",
    "Frame",
    "root_io",
    "sio_io",
    "reading",
]
