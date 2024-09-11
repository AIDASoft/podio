#!/usr/bin/env python3
"""Small test case for checking DataSource based creating RDataFrames is accessible from python"""

import ROOT
from podio.data_source import CreateDataFrame

if ROOT.gSystem.Load("libTestDataModelDict") < 0:  # noqa: E402
    raise RuntimeError("Could not load TestDataModel dictionary")

input_file = "example_frame.root"  # pylint: disable-msg=C0103
rdf = CreateDataFrame(input_file)

assert rdf.Count().GetValue() == 10
