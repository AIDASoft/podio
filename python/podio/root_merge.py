#!/usr/bin/env python3
"""Utilities for merging podio ROOT files (TTree and RNTuple)."""

import os
import tempfile

import ROOT

from podio.frame import Frame
from podio.utils import convert_to_str_paths
from podio.root_io import Reader, RNTupleReader, RNTupleWriter, Writer


def merge_files(output_file, input_files, metadata="first", compression=101):
    """Merge podio ROOT files.

    Uses ROOT's TFileMerger directly for the event data (both TTree and
    RNTuple), then rewrites the ``metadata`` category correctly
    (with the ``MergedInputFiles`` parameter set) via an incremental merge.

    All input files must have been written with the same category and
    collection layout.

    Args:
        output_file (str or Path): Path of the output file to create.
        input_files (list[str] or list[Path]): Ordered list of input files.
        metadata (str): How to handle the ``metadata`` Frame category.
            ``"first"``  -- copy only the first file's entry (default).
            ``"all"``    -- copy entries from every file.
            ``"none"``   -- omit the metadata category entirely.
        compression (int): ROOT compression settings for the output file.
            The format is ``<algorithm>*100 + <level>``, where algorithm is
            1 (ZLIB), 2 (LZMA), 4 (LZ4) or 5 (ZSTD), and level is 1-9.
            Defaults to 101 (ZLIB level 1).  Pass 0 to use the compression
            of the first input file.

    Raises:
        ValueError: If *input_files* is empty or *metadata* is not one of the
            accepted values.
        RuntimeError: If a file cannot be opened or the merge fails.
    """
    if not input_files:
        raise ValueError("input_files must not be empty")

    input_files = [str(p) for p in convert_to_str_paths(input_files)]
    output_file = str(convert_to_str_paths(output_file)[0])

    if compression == 0:
        compression = _first_file_compression(input_files[0])

    metadata_info = _detect_metadata(input_files[0])
    _main_merge(output_file, input_files, compression, metadata_info)

    if metadata == "none":
        return

    # Prepare the corrected metadata frames
    if metadata_info is not None:
        reader_cls, writer_cls = metadata_info
        src_reader = reader_cls([input_files[0]] if metadata == "first" else input_files)
        frames = list(src_reader.get("metadata"))
    else:
        writer_cls = _detect_writer(input_files[0])
        frames = [Frame()]

    for frame in frames:
        frame.put_parameter("MergedInputFiles", list(input_files))

    tmp_fd, tmp_path = tempfile.mkstemp(suffix=".root")
    os.close(tmp_fd)
    try:
        tmp_writer = writer_cls(tmp_path)
        for frame in frames:
            tmp_writer.write_frame(frame, "metadata")
        tmp_writer.finish()

        if metadata_info is not None:
            # Metadata already exists in inputs: podio_metadata from the main
            # merge already has the right entries. Only merge the metadata
            # category data tree.
            merger = ROOT.TFileMerger(ROOT.kFALSE)
            merger.OutputFile(output_file, "UPDATE")
            merger.AddFile(tmp_path)
            merger.SetFastMethod(ROOT.kTRUE)
            merger.AddObjectNames("metadata")
            if not merger.PartialMerge(
                ROOT.TFileMerger.kAll
                | ROOT.TFileMerger.kOnlyListed
                | ROOT.TFileMerger.kIncremental
            ):
                raise RuntimeError(f"TFileMerger failed adding metadata to {output_file}")
        else:
            # Metadata does not exist in inputs. We need to replace
            # podio_metadata too so the Reader discovers the new category.
            # The temp file only contains the metadata category, so its
            # podio_metadata lacks the fields from the output. Rebuild the
            # temp file with all categories to make the schemas match, then
            # delete the old podio_metadata and merge the new one.
            _rebuild_metadata(output_file, tmp_path, writer_cls, frames)
    finally:
        if os.path.exists(tmp_path):
            os.unlink(tmp_path)


def _first_file_compression(filename):
    """Return the compression settings of *filename* (like hadd -ff)."""
    first_file = ROOT.TFile.Open(filename)
    if first_file and not first_file.IsZombie():
        compression = first_file.GetCompressionSettings()
        first_file.Close()
        return compression
    return 0


def _main_merge(output_file, input_files, compression, metadata_info):
    """Merge all objects from all input files into *output_file*.

    When metadata already exists in the inputs, skip it during the main
    merge and write it correctly afterwards, to avoid duplicates.
    """
    merger = ROOT.TFileMerger(ROOT.kFALSE)
    merger.OutputFile(output_file, ROOT.kTRUE, compression)
    merger.SetFastMethod(ROOT.kTRUE)
    for fp in input_files:
        merger.AddFile(fp)
    flags = ROOT.TFileMerger.kAll
    if metadata_info is not None:
        merger.AddObjectNames("metadata")
        flags |= ROOT.TFileMerger.kSkipListed
    if not merger.PartialMerge(flags):
        raise RuntimeError(f"TFileMerger failed merging into {output_file}")


def _rebuild_metadata(output_file, tmp_path, writer_cls, frames):
    """Rebuild temp file with all categories so podio_metadata schemas match.

    The RNTuple merger (and TTree merger for podio_metadata) require
    matching field/branch structures between source and target.  Writing
    one frame for each category from the merged output into the temp file
    ensures the temp file's podio_metadata has all the same fields.
    """
    reader_cls = Reader if writer_cls is Writer else RNTupleReader
    src_reader = reader_cls([output_file])

    # Reopen the temp file with a writer that includes all categories
    tmp_writer = writer_cls(tmp_path)
    for frame in frames:
        tmp_writer.write_frame(frame, "metadata")
    for cat in src_reader.categories:
        it = iter(src_reader.get(cat))
        try:
            tmp_writer.write_frame(next(it), cat)
        except StopIteration:
            pass
    tmp_writer.finish()
    del src_reader

    # Delete old podio_metadata so it can be replaced
    out_f = ROOT.TFile.Open(output_file, "UPDATE")
    if not out_f or out_f.IsZombie():
        raise RuntimeError(f"Cannot open for UPDATE: {output_file}")
    try:
        out_f.Delete("podio_metadata;*")
        out_f.Write("", ROOT.TObject.kOverwrite)
    finally:
        out_f.Close()

    # Merge metadata + podio_metadata from the rebuilt temp file
    merger = ROOT.TFileMerger(ROOT.kFALSE)
    merger.OutputFile(output_file, "UPDATE")
    merger.AddFile(tmp_path)
    merger.SetFastMethod(ROOT.kTRUE)
    merger.AddObjectNames("metadata")
    merger.AddObjectNames("podio_metadata")
    if not merger.PartialMerge(
        ROOT.TFileMerger.kAll | ROOT.TFileMerger.kOnlyListed | ROOT.TFileMerger.kIncremental
    ):
        raise RuntimeError(f"TFileMerger failed adding metadata to {output_file}")


def _detect_metadata(filename):
    """Return (reader_cls, writer_cls) if 'metadata' exists, or None.

    Checks both TTrees and RNTuples since metadata may be stored in either
    format depending on how the file was written.
    """
    root_file = ROOT.TFile.Open(filename)
    if not root_file or root_file.IsZombie():
        raise RuntimeError(f"Cannot open file: {filename}")
    try:
        for elem in root_file.GetListOfKeys():
            if elem.GetName() != "metadata":
                continue
            cls = elem.GetClassName()
            if "TTree" in cls:
                return (Reader, Writer)
            if "RNTuple" in cls:
                return (RNTupleReader, RNTupleWriter)
    finally:
        root_file.Close()
    return None


def _detect_writer(filename):
    """Return the writer class (Writer or RNTupleWriter) for the file backend."""
    root_file = ROOT.TFile.Open(filename)
    if not root_file or root_file.IsZombie():
        raise RuntimeError(f"Cannot open file: {filename}")
    try:
        for elem in root_file.GetListOfKeys():
            if "RNTuple" in elem.GetClassName():
                return RNTupleWriter
    finally:
        root_file.Close()
    return Writer
