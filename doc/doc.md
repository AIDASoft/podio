# Introduction

PODIO, or plain-old-data I/O, is a C++ library to support the creation and handling of data models in particle physics. It is based on the idea of employing plain-old-data (POD) data structures wherever possible, while avoiding deep-object hierarchies and virtual inheritance. This is to both improve runtime performance and simplify the implementation of persistency services.

At the same time it provides the necessary high-level functionality to the physicist, such as support for inter-object relations, and automatic memory-management. In addition, it provides a (ROOT assisted) Python interface. To simplify the creation of efficient data models, PODIO employs code generation from a simple yaml-based markup syntax.

To support the usage of modern software technologies, PODIO was written with concurrency in mind and gives basic support for vectorization technologies.

This document describes first how to define and create a specific data model, then how to use the created data. Afterwards it will explain the overall design and a few of the technical details of the implementation.

Many of the design choices are inspired by previous experience of the [LCIO package](http://lcio.desy.de/) used for the studies of the international linear collider, and the [Gaudi Object Description](http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DataDictionary/Documents/GaudiObjDesc_docu.pdf) applied in the LHCb collaboration at the LHC.

# Quick-start

An up-to-date installation and quick start guide for the impatient user can be found on the [PODIO github page](https://github.com/AIDASoft/podio).
