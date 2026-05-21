#!/usr/bin/env python3
"""Test to exercise DataSource thoroughly"""

import sys
import ROOT
from podio.data_source import CreateDataFrame  # pylint: disable=import-error, no-name-in-module

input_file = sys.argv[1] if len(sys.argv) > 1 else "example_frame.root"
snapshot_file = sys.argv[2] if len(sys.argv) > 2 else "datasource_snapshot.root"

if ROOT.gSystem.Load("libTestDataModelDict") < 0:
    raise RuntimeError("Could not load TestDataModel dictionary")

ROOT.gInterpreter.ProcessLine("using namespace ROOT::VecOps;")

ROOT.gInterpreter.Declare(
    """
#include <datamodel/ExampleHitCollection.h>
#include <datamodel/ExampleClusterCollection.h>
#include <datamodel/EventInfoCollection.h>
#include <datamodel/TestLinkCollection.h>
"""
)

# Declare helpers that extract quantities from the test collections
ROOT.gInterpreter.Declare(
    """
RVec<double> getHitEnergies(const ExampleHitCollection& hits) {
    RVec<double> v;
    v.reserve(hits.size());
    for (const auto& h : hits) {
        v.push_back(h.energy());
    }
    return v;
}

RVec<double> getClusterEnergies(const ExampleClusterCollection& clusters) {
    RVec<double> v;
    v.reserve(clusters.size());
    for (const auto& c : clusters) {
        v.push_back(c.energy());
    }
    return v;
}

int getEventNumber(const EventInfoCollection& info) {
    return info[0].Number();
}

RVec<double> getCompositeClusterHitEnergies(const ExampleClusterCollection& clusters) {
    RVec<double> v;
    for (const auto& hit : clusters[2].Hits()) {
        v.push_back(hit.energy());
    }
    return v;
}

RVec<double> getSubClusterEnergies(const ExampleClusterCollection& clusters) {
    RVec<double> v;
    for (const auto& sub : clusters[2].Clusters()) {
        v.push_back(sub.energy());
    }
    return v;
}

// Follow each link From (hit) and To (cluster) relations and return:
//   [weight0, hit_energy0, cluster_energy0, weight1, hit_energy1, cluster_energy1]
RVec<double> getLinkInfo(const TestLinkCollection& links) {
    RVec<double> v;
    for (const auto& link : links) {
        v.push_back(link.getWeight());
        v.push_back(link.getFrom().energy());
        v.push_back(link.getTo().energy());
    }
    return v;
}
"""
)

rdf = CreateDataFrame(input_file)

assert rdf.Count().GetValue() == 10, f"Expected 10 events in {input_file}"

rdf = (
    rdf.Define("hit_energies", "getHitEnergies(hits)")
    .Define("hit_energy_0", "hit_energies[0]")
    .Define("hit_energy_1", "hit_energies[1]")
    .Define("hit_energy_sum", "Sum(hit_energies)")
    .Define("cluster_energies", "getClusterEnergies(clusters)")
    .Define("cluster_energy_0", "cluster_energies[0]")
    .Define("cluster_energy_1", "cluster_energies[1]")
    .Define("cluster_energy_2", "cluster_energies[2]")
    .Define("event_number", "getEventNumber(info)")
    .Define("composite_hit_energies", "getCompositeClusterHitEnergies(clusters)")
    .Define("composite_hit_energy_0", "composite_hit_energies[0]")
    .Define("composite_hit_energy_1", "composite_hit_energies[1]")
    .Define("sub_cluster_energies", "getSubClusterEnergies(clusters)")
    .Define("sub_cluster_energy_0", "sub_cluster_energies[0]")
    .Define("sub_cluster_energy_1", "sub_cluster_energies[1]")
    .Define("link_info", "getLinkInfo(links)")
    .Define("link0_weight", "link_info[0]")
    .Define("link0_from_energy", "link_info[1]")
    .Define("link0_to_energy", "link_info[2]")
    .Define("link1_weight", "link_info[3]")
    .Define("link1_from_energy", "link_info[4]")
    .Define("link1_to_energy", "link_info[5]")
)

# print(rdf.Describe())
# print(rdf.Display(["event_number", "hit_energy_0", "cluster_energy_2", "composite_hit_energy_0", "sub_cluster_energy_0"]).AsString())

# 10 events, i = 0..9
# hit_energy_0 = 23+i
mean_hit0 = rdf.Mean("hit_energy_0").GetValue()
assert abs(mean_hit0 - 27.5) < 1e-9, f"Mean of hit_energy_0 should be 27.5, got {mean_hit0}"

# cluster_energy_2 = 35+2*i
mean_clu2 = rdf.Mean("cluster_energy_2").GetValue()
assert abs(mean_clu2 - 44.0) < 1e-9, f"Mean of cluster_energy_2 should be 44.0, got {mean_clu2}"

# event_number = i
mean_evtnum = rdf.Mean("event_number").GetValue()
assert abs(mean_evtnum - 4.5) < 1e-9, f"Mean of event_number should be 4.5, got {mean_evtnum}"

# composite_hit_energy_0 follows clusters[2].Hits()[0] == hits[0], energy = 23+i -> Mean = 27.5
mean_chit0 = rdf.Mean("composite_hit_energy_0").GetValue()
assert (
    abs(mean_chit0 - 27.5) < 1e-9
), f"Mean of composite_hit_energy_0 (cluster->hit relation) should be 27.5, got {mean_chit0}"

# sub_cluster_energy_0 follows clusters[2].Clusters()[0] == clusters[0], energy = 23+i -> Mean = 27.5
mean_sub0 = rdf.Mean("sub_cluster_energy_0").GetValue()
assert (
    abs(mean_sub0 - 27.5) < 1e-9
), f"Mean of sub_cluster_energy_0 (cluster->cluster relation) should be 27.5, got {mean_sub0}"

# link0_weight is always 0.0, link1_weight is always 0.5
mean_w0 = rdf.Mean("link0_weight").GetValue()
assert abs(mean_w0 - 0.0) < 1e-9, f"Mean of link0_weight should be 0.0, got {mean_w0}"
mean_w1 = rdf.Mean("link1_weight").GetValue()
assert abs(mean_w1 - 0.5) < 1e-9, f"Mean of link1_weight should be 0.5, got {mean_w1}"

# link0 goes hits[0]->clusters[1]: from-energy = 23+i -> Mean = 27.5,
#                                   to-energy   = 12+i -> Mean = 16.5
mean_l0from = rdf.Mean("link0_from_energy").GetValue()
assert (
    abs(mean_l0from - 27.5) < 1e-9
), f"Mean of link0_from_energy (link->hit relation) should be 27.5, got {mean_l0from}"
mean_l0to = rdf.Mean("link0_to_energy").GetValue()
assert (
    abs(mean_l0to - 16.5) < 1e-9
), f"Mean of link0_to_energy (link->cluster relation) should be 16.5, got {mean_l0to}"

rdf_even = rdf.Filter("event_number % 2 == 0")
assert rdf_even.Count().GetValue() == 5, "Expected 5 even-numbered events after filter"

columns = [
    "event_number",
    "hit_energy_0",
    "hit_energy_1",
    "hit_energy_sum",
    "cluster_energy_0",
    "cluster_energy_1",
    "cluster_energy_2",
    "composite_hit_energy_0",
    "composite_hit_energy_1",
    "sub_cluster_energy_0",
    "sub_cluster_energy_1",
    "link0_weight",
    "link0_from_energy",
    "link0_to_energy",
    "link1_weight",
    "link1_from_energy",
    "link1_to_energy",
]

rdf.Snapshot("events", snapshot_file, columns)

print(f"All assertions passed, snapshot written to {snapshot_file}")
