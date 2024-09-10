#!/usr/bin/env python3
"""Unit tests for python bindings of podio::Frame"""

import unittest

# pylint: disable=import-error
from ROOT import ExampleHitCollection

from podio.frame import Frame

# using root_io as that should always be present regardless of which backends are built
from podio.root_io import Reader


# The expected collections in each frame
EXPECTED_COLL_NAMES = {
    "arrays",
    "WithVectorMember",
    "info",
    "fixedWidthInts",
    "mcparticles",
    "moreMCs",
    "mcParticleRefs",
    "hits",
    "hitRefs",
    "clusters",
    "refs",
    "refs2",
    "OneRelation",
    "userInts",
    "userDoubles",
    "WithNamespaceMember",
    "WithNamespaceRelation",
    "WithNamespaceRelationCopy",
    "emptyCollection",
    "emptySubsetColl",
}
# The expected collections from the extension (only present in the other_events category)
EXPECTED_EXTENSION_COLL_NAMES = {
    "extension_Contained",
    "extension_ExternalComponent",
    "extension_ExternalRelation",
    "VectorMemberSubsetColl",
    "interface_examples",
}

# The expected parameter names in each frame
EXPECTED_PARAM_NAMES = {
    "anInt",
    "UserEventWeight",
    "UserEventName",
    "SomeVectorData",
    "SomeValue",
}


class FrameTest(unittest.TestCase):
    """General unittests for python bindings of the Frame"""

    def test_frame_invalid_access(self):
        """Check that the advertised exceptions are raised on invalid access."""
        # Create an empty Frame here
        frame = Frame()
        with self.assertRaises(KeyError):
            _ = frame.get("NonExistantCollection")

        with self.assertRaises(KeyError):
            _ = frame.get_parameter("NonExistantParameter")

        with self.assertRaises(ValueError):
            collection = [1, 2, 4]
            _ = frame.put(collection, "invalid_collection_type")

    def test_frame_get_name_invalid_token(self):
        """Check that trying to get the collection name raises an exception if
        the collection is not known to the frame"""
        frame = Frame()
        with self.assertRaises(KeyError):
            _ = frame.getName(42)

        with self.assertRaises(KeyError):
            coll = ExampleHitCollection()
            _ = frame.getName(coll)

    def test_frame_put_collection(self):
        """Check that putting a collection works as expected"""
        frame = Frame()
        self.assertEqual(frame.getAvailableCollections(), tuple())

        hits = ExampleHitCollection()
        hits.create()
        hits2 = frame.put(hits, "hits_from_python")
        self.assertEqual(frame.getAvailableCollections(), tuple(["hits_from_python"]))
        # The original collection is gone at this point, and ideally just leaves an
        # empty shell
        self.assertEqual(len(hits), 0)
        # On the other hand the return value of put has the original content
        self.assertEqual(len(hits2), 1)

    def test_frame_put_parameters(self):
        """Check that putting a parameter works as expected"""
        frame = Frame()
        self.assertEqual(frame.parameters, tuple())

        frame.put_parameter("a_string_param", "a string")
        self.assertEqual(frame.parameters, tuple(["a_string_param"]))
        self.assertEqual(frame.get_parameter("a_string_param"), "a string")

        frame.put_parameter("float_param", 3.14)
        self.assertEqual(frame.get_parameter("float_param"), 3.14)

        frame.put_parameter("int", 42)
        self.assertEqual(frame.get_parameter("int"), 42)

        frame.put_parameter("string_vec", ["a", "b", "cd"])
        str_vec = frame.get_parameter("string_vec")
        self.assertEqual(len(str_vec), 3)
        self.assertEqual(str_vec, ["a", "b", "cd"])

        frame.put_parameter("more_ints", [1, 2345])
        int_vec = frame.get_parameter("more_ints")
        self.assertEqual(len(int_vec), 2)
        self.assertEqual(int_vec, [1, 2345])

        frame.put_parameter("float_vec", [1.23, 4.56, 7.89])
        vec = frame.get_parameter("float_vec", as_type="double")
        self.assertEqual(len(vec), 3)
        self.assertEqual(vec, [1.23, 4.56, 7.89])

        frame.put_parameter("real_float_vec", [1.23, 4.56, 7.89], as_type="float")
        f_vec = frame.get_parameter("real_float_vec", as_type="float")
        self.assertEqual(len(f_vec), 3)
        self.assertEqual(vec, [1.23, 4.56, 7.89])

        frame.put_parameter("float_as_float", 3.14, as_type="float")
        self.assertAlmostEqual(frame.get_parameter("float_as_float"), 3.14, places=5)


class FrameReadTest(unittest.TestCase):
    """Unit tests for the Frame python bindings for Frames read from file.

    NOTE: The assumption is that the Frame has been written by tests/write_frame.h
    """

    def setUp(self):
        """Open the file and read in the first frame internally.

        Reading only one event/Frame of each category here as looping and other
        basic checks are already handled by the Reader tests
        """
        reader = Reader("root_io/example_frame.root")
        self.event = reader.get("events")[0]
        self.other_event = reader.get("other_events")[7]

    def test_frame_collections(self):
        """Check that all expected collections are available."""
        self.assertEqual(set(self.event.getAvailableCollections()), EXPECTED_COLL_NAMES)
        self.assertEqual(
            set(self.other_event.getAvailableCollections()),
            EXPECTED_COLL_NAMES.union(EXPECTED_EXTENSION_COLL_NAMES),
        )

        # Not going over all collections here, as that should all be covered by the
        # c++ test cases; Simply picking a few and doing some basic tests
        mc_particles = self.event.get("mcparticles")
        self.assertEqual(mc_particles.getValueTypeName().data(), "ExampleMC")
        self.assertEqual(len(mc_particles), 10)
        self.assertEqual(len(mc_particles[0].daughters()), 4)

        mc_particle_refs = self.event.get("mcParticleRefs")
        self.assertTrue(mc_particle_refs.isSubsetCollection())
        self.assertEqual(len(mc_particle_refs), 10)

        fixed_w_ints = self.event.get("fixedWidthInts")
        self.assertEqual(len(fixed_w_ints), 3)
        # Python has no concept of fixed width integers...
        max_vals = fixed_w_ints[0]
        self.assertEqual(max_vals.fixedInteger64(), 2**63 - 1)
        self.assertEqual(max_vals.fixedU64(), 2**64 - 1)

    def test_frame_parameters(self):
        """Check that all expected parameters are available."""
        self.assertEqual(set(self.event.parameters), EXPECTED_PARAM_NAMES)
        self.assertEqual(set(self.other_event.parameters), EXPECTED_PARAM_NAMES)

        self.assertEqual(self.event.get_parameter("anInt"), 42)
        self.assertEqual(self.other_event.get_parameter("anInt"), 42 + 107)

        self.assertEqual(self.event.get_parameter("UserEventWeight"), 0)
        self.assertEqual(self.other_event.get_parameter("UserEventWeight"), 100.0 * 107)

        self.assertEqual(self.event.get_parameter("UserEventName"), " event_number_0")
        self.assertEqual(self.other_event.get_parameter("UserEventName"), " event_number_107")

        with self.assertRaises(ValueError):
            # Parameter name is available with multiple types
            _ = self.event.get_parameter("SomeVectorData")

        with self.assertRaises(ValueError):
            # Parameter not available as float (only int and string)
            _ = self.event.get_parameter("SomeValue", as_type="float")

        self.assertEqual(self.event.get_parameter("SomeVectorData", as_type="int"), [1, 2, 3, 4])
        self.assertEqual(
            self.event.get_parameter("SomeVectorData", as_type="str"),
            ["just", "some", "strings"],
        )
        # as_type='float' will also retrieve double values (if the name is unambiguous)
        self.assertEqual(self.event.get_parameter("SomeVectorData", as_type="float"), [0.0, 0.0])

    def test_frame_get_name(self):
        """Check that retrieving the name of a collection works as expected"""
        mc_particles = self.event.get("mcparticles")
        self.assertEqual(self.event.getName(mc_particles), "mcparticles")
        self.assertEqual(self.event.getName(mc_particles.getID()), "mcparticles")
