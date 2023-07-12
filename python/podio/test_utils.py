#!/usr/bin/env python3
"""Utilities for python unittests"""

import os

SKIP_SIO_TESTS = os.environ.get("SKIP_SIO_TESTS", "1") == "1"

import ROOT
ROOT.gSystem.Load("libTestDataModelDict.so")
from ROOT import ExampleHitCollection, ExampleClusterCollection

from podio.frame import Frame


def create_hit_collection():
  """Create a simple hit collection with two hits for testing"""
  hits = ExampleHitCollection()
  hits.create(0xBAD, 0.0, 0.0, 23.0)
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
  frame.put(create_hit_collection(), "hits_from_python")
  frame.put(create_cluster_collection(), "cluster_from_python")

  return frame


def write_file(WriterT, filename):
  """Write a file using the given Writer type and put one Frame into it under
  the events category
  """
  writer = WriterT(filename)
  event = create_frame()
  writer.write_frame(event, "events")
