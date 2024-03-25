"""cppyy pythonizations for podio"""

from importlib import import_module
from pkgutil import walk_packages
from .utils.pythonizer import Pythonizer


def load_pythonizations(namespace):
    """Register all available pythonizations for a given namespace"""
    module_names = [name for _, name, _ in walk_packages(__path__) if not name.startswith("test_")]
    for module_name in module_names:
        import_module(__name__ + "." + module_name)
    pythonizers = sorted(Pythonizer.__subclasses__(), key=lambda x: x.priority())
    for i in pythonizers:
        i.register(namespace)
