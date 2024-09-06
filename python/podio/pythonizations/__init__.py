"""cppyy pythonizations for podio"""

from importlib import import_module
from pkgutil import iter_modules
from os import path
from .utils.pythonizer import Pythonizer


def load_pythonizations(namespace):
    """Register all available pythonizations for a given namespace"""
    pythonizations_dir = path.dirname(__file__)
    # find only direct submodules of the current module
    module_names = [name for _, name, _ in iter_modules([pythonizations_dir])]
    for module_name in module_names:
        import_module(__name__ + "." + module_name)
    pythonizers = sorted(Pythonizer.__subclasses__(), key=lambda x: x.priority())
    for i in pythonizers:
        i.register(namespace)
