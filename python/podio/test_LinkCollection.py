#!/usr/bin/env python3
"""Unit tests for LinkCollection Python accessibility

These tests verify that LinkCollection is accessible from the podio module
and that template instantiation via subscript notation works, i.e. that

    LinkCollection[FromType, ToType]

produces a usable collection type. This mirrors the use case from EDM packages
like edm4hep where users want to write

    LinkCollection[edm4hep.ReconstructedParticle, edm4hep.MCParticle]

rather than having to use ROOT.LinkCollection directly.
"""

import unittest

from ROOT import (
    ExampleHit,
    ExampleCluster,
    ExampleHitCollection,
    ExampleClusterCollection,
    TestLinkCollection,
)

from podio import LinkCollection


class LinkCollectionImportTest(unittest.TestCase):
    """Tests that LinkCollection is accessible from the podio module"""

    def test_link_collection_accessible(self):
        """Test that LinkCollection can be imported and exists, i.e. is not None"""
        self.assertIsNotNone(LinkCollection)

        coll_type = LinkCollection[ExampleHit, ExampleCluster]
        self.assertIsNotNone(coll_type)

        # Ensure that defined link collections behave as expected
        coll_type = LinkCollection[ExampleHit, ExampleCluster]
        self.assertEqual(coll_type.__cpp_name__, TestLinkCollection.__cpp_name__)

    def test_link_collection_instantiation(self):
        """Test that a subscripted LinkCollection can be instantiated"""
        coll = LinkCollection[ExampleHit, ExampleCluster]()
        self.assertIsNotNone(coll)
        self.assertEqual(len(coll), 0)

    def test_link_collection_create_links(self):
        """Test that links can be created in a subscripted LinkCollection instance"""
        hits = ExampleHitCollection()
        hit = hits.create()

        clusters = ExampleClusterCollection()
        cluster = clusters.create()

        coll = LinkCollection[ExampleHit, ExampleCluster]()
        link = coll.create()
        link.setFrom(hit)
        link.setTo(cluster)

        self.assertEqual(len(coll), 1)
