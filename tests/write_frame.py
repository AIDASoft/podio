#!/usr/bin/env python3
"""Utilities for python unittests"""

import importlib


import ROOT

if ROOT.gSystem.Load("libTestDataModelDict") < 0:  # noqa: E402
    raise RuntimeError("Could not load TestDataModel dictionary")

from ROOT import (  # pylint: disable=wrong-import-position
    ExampleHitCollection,
    ExampleClusterCollection,
    TestLinkCollection,
    TestInterfaceLinkCollection,
    TypeWithEnergy,
)  # noqa: E402

from podio import Frame  # pylint: disable=wrong-import-position


def create_hit_collection():
    """Create a simple hit collection with two hits for testing"""
    hits = ExampleHitCollection()
    hits.create(0xBAD, 0.0, 0.0, 0.0, 23.0)
    hits.create(0xCAFFEE, 1.0, 0.0, 0.0, 12.0)

    return hits


def create_cluster_collection():
    """Create a simple cluster collection with two clusters"""
    clusters = ExampleClusterCollection()
    clu0 = clusters.create()
    clu0.energy(3.14)
    clu1 = clusters.create()
    clu1.energy(1.23)

    return clusters


def create_link_collection(clusters, hits):
    """Create a collection of links"""
    links = TestLinkCollection()
    link = links.create()
    link.setFrom(hits[0])
    link.setTo(clusters[0])

    return links


def create_link_with_interface_collection(clusters, hits):
    """Create a collection of links with an interface type"""
    links = TestInterfaceLinkCollection()

    link = links.create()
    link.setFrom(clusters[0])
    link.setTo(TypeWithEnergy(hits[0]))

    link = links.create()
    link.setFrom(clusters[1])
    link.setTo(TypeWithEnergy(clusters[0]))

    return links


def create_frame():
    """Create a frame with an ExampleHit and an ExampleCluster collection"""
    frame = Frame()
    hits = create_hit_collection()
    hits = frame.put(hits, "hits_from_python")
    clusters = create_cluster_collection()
    clusters = frame.put(clusters, "clusters_from_python")
    frame.put(create_link_collection(clusters, hits), "links_from_python")
    frame.put(
        create_link_with_interface_collection(clusters, hits), "links_with_interfaces_from_python"
    )

    frame.put_parameter("an_int", 42)
    frame.put_parameter("some_floats", [1.23, 7.89, 3.14])
    frame.put_parameter("greetings", ["from", "python"])
    frame.put_parameter("real_float", 3.14, as_type="float")
    frame.put_parameter("more_real_floats", [1.23, 4.56, 7.89], as_type="float")

    return frame


def write_file(writer_type, filename):
    """Write a file using the given Writer type and put one Frame into it under
    the events category
    """
    io_backend, writer_name = writer_type.split(".")
    io_module = importlib.import_module(f"podio.{io_backend}")

    writer = getattr(io_module, writer_name)(filename)
    event = create_frame()
    writer.write_frame(event, "events")


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("outputfile", help="Output file name")
    parser.add_argument("writer", help="The writer type to use")

    args = parser.parse_args()

    io_format = args.outputfile.split(".")[-1]

    write_file(args.writer, args.outputfile)
