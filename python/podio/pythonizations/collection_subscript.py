"""Pythonize subscript operation for collections"""

import cppyy
from .utils.pythonizer import Pythonizer


class CollectionSubscriptPythonizer(Pythonizer):
    """Bound-check `__getitem__` for classes derived from `podio.CollectionBase`"""

    @classmethod
    def priority(cls):
        """
        No special requirements for order of applying

        Returns:
            int: Priority.
        """
        return 50

    @classmethod
    def filter(cls, class_, name):
        """
        Filters-out classes non derived from `podio.CollectionBase`.

        Args:
            class_ (type): Class object.
            name (str): Name of the class.

        Returns:
            bool: True if class is derived from `podio.CollectionBase` and should be pythonized.
        """
        return issubclass(class_, cppyy.gbl.podio.CollectionBase)

    @classmethod
    def modify(cls, class_, name):
        """
        Raise an `IndexError` exception if an index is invalid.
        The `__getitem__` will return immutable datatype objects instead of the mutable ones.

        Args:
            class_ (type): Class object.
            name (str): Name of the class.
        """

        def get_item(self, i):
            try:
                return self.at(i)
            except cppyy.gbl.std.out_of_range:
                raise IndexError("collection index out of range") from None

        class_.__getitem__ = get_item
