#!/usr/bin/env python3
"""Unit tests for podio readers"""


class ReaderTestCaseMixin:
    """Common unittests for readers.

    Inheriting actual test cases have to inherit from this and unittest.TestCase.
    All test cases assume that the files are produced with the tests/write_frame.h
    functionality. The following members have to be setup and initialized by the
    inheriting test cases:
    - reader: a podio reader
    """

    def test_categories(self):
        """Make sure that the categories are as expected"""
        reader_cats = self.reader.categories
        self.assertEqual(len(reader_cats), 2)

        for cat in ("events", "other_events"):
            self.assertTrue(cat in reader_cats)

    def test_frame_iterator_valid_category(self):
        """Check that the returned iterators returned by Reader.get behave as expected."""
        # NOTE: very basic iterator tests only, content tests are done elsewhere
        frames = self.reader.get("other_events")
        self.assertEqual(len(frames), 10)

        i = 0
        for frame in self.reader.get("events"):
            # Rudimentary check here only to see whether we got the right frame
            self.assertEqual(frame.get_parameter("UserEventName"), f" event_number_{i}")
            i += 1
        self.assertEqual(i, 10)

        # Out of bound access should not work
        with self.assertRaises(IndexError):
            _ = frames[10]
        with self.assertRaises(IndexError):
            _ = frames[-11]

        # Again only rudimentary checks
        frame = frames[7]
        self.assertEqual(frame.get_parameter("UserEventName"), " event_number_107")
        # Valid negative indexing
        frame = frames[-2]
        self.assertEqual(frame.get_parameter("UserEventName"), " event_number_108")
        # jumping back again also works
        frame = frames[3]
        self.assertEqual(frame.get_parameter("UserEventName"), " event_number_103")

        # Looping starts from where we left, i.e. here we have 6 frames left
        i = 0
        for _ in frames:
            i += 1
        self.assertEqual(i, 6)

    def test_frame_iterator_invalid_category(self):
        """Make sure non existant Frames are handled gracefully"""
        non_existant = self.reader.get("non-existant")
        self.assertEqual(len(non_existant), 0)

        # Indexed access should obviously not work
        with self.assertRaises(IndexError):
            _ = non_existant[0]

        # Loops should never be entered
        i = 0
        for _ in non_existant:
            i += 1
        self.assertEqual(i, 0)


class LegacyReaderTestCaseMixin:
    """Common test cases for the legacy readers python bindings.

    These tests assume that input files are produced with the write_test.h header
    and that inheriting test cases inherit from unittest.TestCase as well.
    Additionally they have to have an initialized reader as a member.

    NOTE: Since the legacy readers also use the BaseReaderMixin, many of the
    invalid access test cases are already covered by the ReaderTestCaseMixin and
    here we simply focus on the slightly different happy paths
    """

    def test_categories(self):
        """Make sure the legacy reader returns only one category"""
        cats = self.reader.categories
        self.assertEqual(("events",), cats)

    def test_frame_iterator(self):
        """Make sure the FrameIterator works."""
        frames = self.reader.get("events")
        self.assertEqual(len(frames), 2000)

        for i, frame in enumerate(frames):
            # Rudimentary check here only to see whether we got the right frame
            self.assertEqual(frame.get_parameter("UserEventName"), f" event_number_{i}")
            # Only check a few Frames here
            if i > 10:
                break

        # Index based access
        frame = frames[123]
        self.assertEqual(frame.get_parameter("UserEventName"), " event_number_123")
