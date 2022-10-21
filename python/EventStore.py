"""Legacy import wrapper for EventStore."""

import warnings
warnings.warn("You are using the legacy EventStore import. Switch to 'from podio import EventStore'", FutureWarning)

from podio import EventStore  # noqa: F401 # pylint: disable=wrong-import-position, unused-import
