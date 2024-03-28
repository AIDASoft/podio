"""Pythonize subscript operation for collections"""

import cppyy
from .utils.pythonizer import Pythonizer


class CollectionSubscriptPythonizer(Pythonizer):
    """Bound-check __getitem__ for classes derived from podio::CollectionBase"""

    @classmethod
    def priority(cls):
        return 50

    @classmethod
    def callback(cls, class_, name):
        def get_item(self, i):
            try:
                return self.at(i)
            except cppyy.gbl.std.out_of_range:
                raise IndexError("collection index out of range") from None

        if issubclass(class_, cppyy.gbl.podio.CollectionBase) and hasattr(class_, "at"):
            class_.__getitem__ = get_item
