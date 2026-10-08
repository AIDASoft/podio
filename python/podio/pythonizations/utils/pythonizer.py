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
        """Order in which the pythonizations are applied

        Returns:
            int: Priority
        """

    @classmethod
    @abstractmethod
    def filter(cls, class_, name):
        """
        Abstract classmethod to filter classes to which the pythonizations should be applied

        Args:
            class_ (type): Class object.
            name (str): Name of the class.

        Returns:
            bool: True if class should be pythonized.
        """

    @classmethod
    @abstractmethod
    def modify(cls, class_, name):
        """Abstract classmethod modifying classes to be pythonized

        Args:
            class_ (type): Class object.
            name (str): Name of the class.
        """

    @classmethod
    def register(cls, namespace):
        """Helper method to apply the pythonization to the given namespace

        Args:
            namespace (str): Namespace to by pythonized
        """

        def pythonization_callback(class_, name):
            if cls.filter(class_, name):
                cls.modify(class_, name)

        cppyy.py.add_pythonization(pythonization_callback, namespace)
