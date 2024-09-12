#!/usr/bin/env python3
"""Small test case for checking DataSource based creating RDataFrames is accessible from python"""

import ROOT
from podio.data_source import CreateDataFrame  # pylint: disable=import-error, no-name-in-module

if ROOT.gSystem.Load("libTestDataModelDict") < 0:
    raise RuntimeError("Could not load TestDataModel dictionary")

rdf = CreateDataFrame("example_frame.root")

assert rdf.Count().GetValue() == 10
