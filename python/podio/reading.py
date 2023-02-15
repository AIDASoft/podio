#!/usr/bin/env python3
"""Module for general reading functionality."""

from ROOT import TFile

from podio import root_io
try:
  from podio import sio_io

  def _is_frame_sio_file(filename):
    """Peek into the sio file to determine whether this is a legacy file or not."""
    with open(filename, 'rb') as sio_file:
      first_line = str(sio_file.readline())
      # The SIO Frame writer writes a podio_header_info at the beginning of the
      # file
      return first_line.find('podio_header_info') > 0

except ImportError:
  def _is_frame_sio_file(filename):
    """Stub raising a ValueError"""
    raise ValueError('podio has not been built with SIO support, '
                     'which is necessary to read this file, '
                     'or there is a version mismatch')


def _is_frame_root_file(filename):
  """Peek into the root file to determine whether this is a legacy file or not."""
  file = TFile.Open(filename)
  # The ROOT Frame writer puts a podio_metadata TTree into the file
  return bool(file.Get('podio_metadata'))


def get_reader(filename):
  """Get an appropriate reader for the passed file.

  Args:
      filename (str): The input file

  Returns:
      root_io.[Legacy]Reader, sio_io.[Legacy]Reader: an initialized reader that
          is able to process the input file.

  Raises:
      ValueError: If the file cannot be recognized, or if podio has not been
          built with the necessary backend I/O support
  """
  if filename.endswith('.sio'):
    if _is_frame_sio_file(filename):
      return sio_io.Reader(filename)
    return sio_io.LegacyReader(filename)

  if filename.endswith('.root'):
    if _is_frame_root_file(filename):
      return root_io.Reader(filename)
    return root_io.LegacyReader(filename)

  raise ValueError('file must end on .root or .sio')
