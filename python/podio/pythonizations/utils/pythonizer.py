"""cppyy pythonizations for podio"""

from abc import ABCMeta, abstractmethod
import cppyy


class Pythonizer(metaclass=ABCMeta):
    """
    Base class to define cppyy pythonization for podio
    """

    @classmethod
    @abstractmethod
    def priority(cls):
        """Order in which the pythonizations are applied"""

    @classmethod
    @abstractmethod
    def callback(cls, class_, name):
        """Pythonization callback"""

    @classmethod
    def register(cls, namespace):
        """Helper method to apply the pythonization to the given namespace"""
        cppyy.py.add_pythonization(cls.callback, namespace)
