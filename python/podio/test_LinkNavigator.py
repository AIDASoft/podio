#!/usr/bin/env python3
"""Unit tests for python bindings of podio::LinkNavigator"""

import unittest

from ROOT import ExampleHitCollection, ExampleClusterCollection, TestLinkCollection

from podio import LinkNavigator, ReturnFrom, ReturnTo


class LinkNavigatorTest(unittest.TestCase):
    """Unit tests for the LinkNavigator wrapper"""

    def test_basic_navigation(self):
        """Test basic LinkNavigator creation and usage"""
        hits = ExampleHitCollection()
        hit = hits.create()

        clusters = ExampleClusterCollection()
        cluster = clusters.create()

        links = TestLinkCollection()
        link = links.create()
        link.setFrom(hit)
        link.setTo(cluster)
        link.setWeight(0.5)

        nav = LinkNavigator(links)

        linked = nav.getLinked(hit)
        self.assertEqual(len(linked), 1)
        self.assertEqual(linked[0].o, cluster)
        self.assertAlmostEqual(linked[0].weight, 0.5)

    def test_navigation_reverse_direction(self):
        """Test navigating in the reverse direction"""
        hits = ExampleHitCollection()
        hit = hits.create()

        clusters = ExampleClusterCollection()
        cluster = clusters.create()

        links = TestLinkCollection()
        link = links.create()
        link.setFrom(hit)
        link.setTo(cluster)
        link.setWeight(0.75)

        nav = LinkNavigator(links)

        # Navigate from cluster back to hit
        linked = nav.getLinked(cluster)
        self.assertEqual(len(linked), 1)
        self.assertEqual(linked[0].o, hit)
        self.assertAlmostEqual(linked[0].weight, 0.75)

    def test_multiple_links(self):
        """Test navigation with multiple links"""
        hits = ExampleHitCollection()
        hit = hits.create()

        clusters = ExampleClusterCollection()
        cluster1 = clusters.create()
        cluster2 = clusters.create()

        links = TestLinkCollection()
        link1 = links.create()
        link1.setFrom(hit)
        link1.setTo(cluster1)
        link1.setWeight(0.5)

        link2 = links.create()
        link2.setFrom(hit)
        link2.setTo(cluster2)
        link2.setWeight(0.8)

        nav = LinkNavigator(links)

        linked = nav.getLinked(hit)
        self.assertEqual(len(linked), 2)
        weights = sorted([linked[0].weight, linked[1].weight])
        self.assertAlmostEqual(weights[0], 0.5, places=5)
        self.assertAlmostEqual(weights[1], 0.8, places=5)

    def test_type_error_on_non_link_collection(self):
        """Test that TypeError is raised for non-LinkCollection"""
        hits = ExampleHitCollection()
        with self.assertRaises(TypeError):
            LinkNavigator(hits)

    def test_tag_variables_accessible(self):
        """Test that ReturnFrom and ReturnTo are accessible"""
        # Just check they exist and are the expected types
        self.assertIsNotNone(ReturnFrom)
        self.assertIsNotNone(ReturnTo)
