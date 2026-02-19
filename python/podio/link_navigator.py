#!/usr/bin/env python3
"""Module for the python bindings of the podio::LinkNavigator"""

import ROOT

# Load the LinkNavigator header
if ROOT.gInterpreter.LoadFile("podio/LinkNavigator.h") != 0:
    raise ImportError(
        "Could not load podio/LinkNavigator.h. Make sure it is available on ROOT_INCLUDE_PATH."
    )

from ROOT import podio  # noqa: E402 # pylint: disable=wrong-import-position


# pylint: disable-next=invalid-name
def LinkNavigator(link_collection):
    """Create a LinkNavigator for the given LinkCollection.

    This factory function automatically determines the collection type and
    instantiates the correct C++ LinkNavigator template.

    Args:
        link_collection: A podio LinkCollection instance

    Returns:
        podio::LinkNavigator[LinkCollectionType]: The instantiated navigator

    Raises:
        TypeError: If the passed object is not a LinkCollection

    Example:
        >>> from podio import LinkNavigator
        >>> nav = LinkNavigator(my_link_collection)
        >>> linked_objects = nav.getLinked(some_object)
    """
    coll_type = type(link_collection)

    # Verify this is a LinkCollection by checking for type aliases
    if not hasattr(coll_type, "from_type") or not hasattr(coll_type, "to_type"):
        raise TypeError(
            f"Expected a LinkCollection, but got {coll_type.__cpp_name__}. "
            "The collection must have 'from_type' and 'to_type' type aliases."
        )

    navigator_type = podio.LinkNavigator[coll_type]
    return navigator_type(link_collection)


# Re-export tag variables for same-type link disambiguation
ReturnFrom = podio.ReturnFrom
ReturnTo = podio.ReturnTo
LinkCollection = podio.LinkCollection
