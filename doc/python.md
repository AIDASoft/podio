# Python interface for data models

Podio provides support for a Python interface for the generated data models. The [design choice](design.md) to create Python interface resembling the C++ interface is achieved by generating Python bindings from the C++ interface using
[cppyy](https://cppyy.readthedocs.io/en/latest/index.html). To make pyROOT aware of the bindings, the cppyy functionality bundled with ROOT can be used.

It's important to note that cppyy loads the bindings and presents them lazily at runtime to the Python interpreter, rather than writing Python interface files. Consequently, the Python bindings have a runtime dependencies on ROOT, cppyy and the data model's C++ interface.

To load the Python bindings from a generated C++ model dictionary, first make sure the model's library and headers can be found in `LD_LIBRARY_PATH` and `ROOT_INCLUDE_HEADERS` respectively, then:

```python
import ROOT

res = ROOT.gSystem.Load('libGeneratedModelDict')
if res < 0:
    raise RuntimeError('Failed to load libGeneratedModelDict')
```

For reference usage, see the [Python module of EDM4hep](https://github.com/key4hep/EDM4hep/blob/main/python/edm4hep/__init__.py).

## Pythonizations

Python as a language uses different constructions and conventions than C++, perfectly fine C++ code translated one to one to Python could be clunky by Python's standard. cppyy offers a mechanism called [pythonizations](https://cppyy.readthedocs.io/en/latest/pythonizations.html) to make the resulting bindings more pythonic. Some basic pythonizations are included automatically (for instance `operator[]` is translated to `__getitem__`) but others can be specified by a user.

Podio comes with its own set of pythonizations useful for the data models generated with it. These pythonizations are available in the submodules of the `podio.pythonizations`. For the full list see the [python API](https://key4hep.web.cern.ch/podio/py_api/podio.pythonizations.html#submodules) documentation.
To apply all the provided pythonizations to a `model_namespace` namespace:

```python
from podio.pythonizations import load_pythonizations

load_pythonizations("model_namespace")
```

If only specific pythonizations should be applied:

```python
from  podio.pythonizations import collection_subscript # specific pythonization

collection_subscript.CollectionSubscriptPythonizer.register("model_namespace")
```

### Developing new pythonizations

To be discovered by `load_pythonizations`, any new pythonization should be placed in `podio.pythonizations` and be derived from the abstract class `podio.pythonizations.utils.pythonizer.Pythonizer`.

A pythonization class should implement the following three class methods:

- `priority`: The `load_pythonizations` function applies the pythonizations in increasing order of their `priority`
- `filter`: A predicate to filter out classes to which given pythonization should be applied. See the [cppyy documentation](https://cppyy.readthedocs.io/en/latest/pythonizations.html#python-callbacks).
- `modify`: Applying the modifications to the pythonized classes.

### Considerations

The cppyy pythonizations come with some considerations:

- The general cppyy idea to lazily load only things that are needed applies only partially to the pythonizations. For instance, a pythonization modifying the `collection[]` will be applied the first time a class of `collection` is used, regardless if `collection[]` is actually used.
- Each pythonization is applied to all the entities in a namespace and relies on a conditional mechanism (`filter` method) inside the pythonizations to select entities they modify. With a large number of pythonizations, the overheads will add up and slow down the usage of any class from a pythonized namespace.
- The cppyy bindings hooking to the C++ routines are characterized by high performance compared to ordinary Python code. The pythonizations are written in Python and are executed at ordinary Python code speed.
