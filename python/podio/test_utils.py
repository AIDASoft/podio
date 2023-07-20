#!/usr/bin/env python3
"""Utilities for python unittests"""

import os
import ROOT
ROOT.gSystem.Load("libTestDataModelDict.so")  # noqa: E402
from ROOT import ExampleHitCollection, ExampleClusterCollection  # noqa: E402 # pylint: disable=wrong-import-position

from podio.frame import Frame  # pylint: disable=wrong-import-position


SKIP_SIO_TESTS = os.environ.get("SKIP_SIO_TESTS", "1") == "1"


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


def create_frame():
  """Create a frame with an ExampleHit and an ExampleCluster collection"""
  frame = Frame()
  hits = create_hit_collection()
  frame.put(hits, "hits_from_python")
  clusters = create_cluster_collection()
  frame.put(clusters, "clusters_from_python")

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
  writer = writer_type(filename)
  event = create_frame()
  writer.write_frame(event, "events")
