#!/usr/bin/env python3
"""Unit tests for python bindings of podio::Frame"""

import unittest

from podio.frame import Frame
# using root_io as that should always be present regardless of which backends are built
from podio.root_io import Reader

# The expected collections in each frame
EXPECTED_COLL_NAMES = {
    'arrays', 'WithVectorMember', 'info', 'fixedWidthInts', 'mcparticles',
    'moreMCs', 'mcParticleRefs', 'hits', 'hitRefs', 'clusters', 'refs', 'refs2',
    'OneRelation', 'userInts', 'userDoubles', 'WithNamespaceMember',
    'WithNamespaceRelation', 'WithNamespaceRelationCopy',
    'emptyCollection', 'emptySubsetColl'
    }
# The expected parameter names in each frame
EXPECTED_PARAM_NAMES = {'anInt', 'UserEventWeight', 'UserEventName', 'SomeVectorData'}


class FrameTest(unittest.TestCase):
  """General unittests for for python bindings of the Frame"""
  def test_frame_invalid_access(self):
    """Check that the advertised exceptions are raised on invalid access."""
    # Creat an empty Frame here
    frame = Frame()
    with self.assertRaises(KeyError):
      _ = frame.get('NonExistantCollection')

    with self.assertRaises(KeyError):
      _ = frame.get_parameter('NonExistantParameter')


class FrameReadTest(unittest.TestCase):
  """Unit tests for the Frame python bindings for Frames read from file.

  NOTE: The assumption is that the Frame has been written by tests/write_frame.h
  """
  def setUp(self):
    """Open the file and read in the first frame internally.

    Reading only one event/Frame of each category here as looping and other
    basic checks are already handled by the Reader tests
    """
    reader = Reader('example_frame.root')
    self.event = reader.get('events')[0]
    self.other_event = reader.get('other_events')[7]

  def test_frame_collections(self):
    """Check that all expected collections are available."""
    self.assertEqual(set(self.event.collections), EXPECTED_COLL_NAMES)
    self.assertEqual(set(self.other_event.collections), EXPECTED_COLL_NAMES)

    # Not going over all collections here, as that should all be covered by the
    # c++ test cases; Simply picking a few and doing some basic tests
    mc_particles = self.event.get('mcparticles')
    self.assertEqual(mc_particles.getValueTypeName(), 'ExampleMC')
    self.assertEqual(len(mc_particles), 10)
    self.assertEqual(len(mc_particles[0].daughters()), 4)

    mc_particle_refs = self.event.get('mcParticleRefs')
    self.assertTrue(mc_particle_refs.isSubsetCollection())
    self.assertEqual(len(mc_particle_refs), 10)

    fixed_w_ints = self.event.get('fixedWidthInts')
    self.assertEqual(len(fixed_w_ints), 3)
    # Python has no concept of fixed width integers...
    max_vals = fixed_w_ints[0]
    self.assertEqual(max_vals.fixedInteger64(), 2**63 - 1)
    self.assertEqual(max_vals.fixedU64(), 2**64 - 1)

  def test_frame_parameters(self):
    """Check that all expected parameters are available."""
    self.assertEqual(set(self.event.parameters), EXPECTED_PARAM_NAMES)
    self.assertEqual(set(self.other_event.parameters), EXPECTED_PARAM_NAMES)

    self.assertEqual(self.event.get_parameter('anInt'), 42)
    self.assertEqual(self.other_event.get_parameter('anInt'), 42 + 107)

    self.assertEqual(self.event.get_parameter('UserEventWeight'), 0)
    self.assertEqual(self.other_event.get_parameter('UserEventWeight'), 100. * 107)

    self.assertEqual(self.event.get_parameter('UserEventName'), ' event_number_0')
    self.assertEqual(self.other_event.get_parameter('UserEventName'), ' event_number_107')

    with self.assertRaises(ValueError):
      # Parameter name is available with multiple types
      _ = self.event.get_parameter('SomeVectorData')

    with self.assertRaises(ValueError):
      # Parameter not available as float (only int and string)
      _ = self.event.get_parameter('SomeVectorData', as_type='float')

    self.assertEqual(self.event.get_parameter('SomeVectorData', as_type='int'), [1, 2, 3, 4])
    self.assertEqual(self.event.get_parameter('SomeVectorData', as_type='str'), ["just", "some", "strings"])
