#!/usr/bin/env python3

import cppyy

cppyy.cppdef("""
namespace podio {
struct CollectionBase {};
}
"""
)


class gInterpreter:
    def LoadFile(self, *_):
        return 1


class gSystem:
    def Load(self, *_):
        return 1

    def DynamicPathName(self, *_):
        return "path"


class TFile:
    pass
