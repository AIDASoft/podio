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
            if i >= len(self):
                raise IndexError("collection index out of range")
            return getitem_raw(self, i)

        if (
            issubclass(class_, cppyy.gbl.podio.CollectionBase)
            and hasattr(class_, "__getitem__")
            and hasattr(class_, "__len__")
        ):
            getitem_raw = class_.__getitem__
            class_.__getitem__ = get_item
