#!/usr/bin/env python3
"""Small test case for checking that utilities work as expected in RDataFrame"""

import sys
import ROOT


ROOT.gInterpreter.Declare("#include <podio/utilities/RootHelpers.h>")
df = ROOT.RDataFrame("events", sys.argv[1])

evtWeights = (
    df.Define(
        "params",
        "podio::root_utils::loadParamsFrom(GPIntKeys, GPIntValues, GPFloatKeys, GPFloatValues,"
        "GPDoubleKeys, GPDoubleValues, GPStringKeys, GPStringValues)",
    )
    .Define("eventweight", 'params.get<float>("UserEventWeight").value()')
    .Histo1D("eventweight")
)

if evtWeights.GetMean() != 450.0:
    print("Parameter values not as expected")
    sys.exit(1)
