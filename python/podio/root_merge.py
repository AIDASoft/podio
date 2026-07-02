#!/usr/bin/env python3
"""Utilities for merging podio ROOT files (TTree and RNTuple)."""

import tempfile

import ROOT

from podio.frame import Frame
from podio.utils import convert_to_str_paths
from podio.reading import get_reader
from podio.root_io import Reader, RNTupleWriter, Writer


def merge_files(input_files, output_file, metadata="first", compression=101):
    """Merge podio ROOT files.

    Uses ROOT's TFileMerger directly for the event data (both TTree and
    RNTuple), then rewrites the metadata category correctly (with the
    MergedInputFiles parameter set) via an incremental merge.

    All input files must have been written with the same category and
    collection layout.

    Args:
        input_files (list[str] or list[Path]): Ordered list of input files.
        output_file (str or Path): Path of the output file to create.
        metadata (str): How to handle the metadata Frame category.
            "first" copies only the first file's entry (default), "all"
            copies entries from every file, "none" omits the metadata
            category entirely.
        compression (int): ROOT compression settings for the output file.
            The format is algorithm*100 + level, where algorithm is
            1 (ZLIB), 2 (LZMA), 4 (LZ4) or 5 (ZSTD), and level is 1-9.
            Defaults to 101 (ZLIB level 1). Pass 0 to use the compression
            of the first input file.

    Raises:
        ValueError: If input_files is empty or metadata is not one of the
            accepted values.
        RuntimeError: If a file cannot be opened or the merge fails.
    """
    if not input_files:
        raise ValueError("input_files must not be empty")

    input_files = [str(p) for p in convert_to_str_paths(input_files)]
    output_file = str(convert_to_str_paths(output_file)[0])

    if compression == 0:
        compression = _first_file_compression(input_files[0])

    has_metadata = _has_metadata(input_files[0])
    _main_merge(input_files, output_file, compression, has_metadata)

    if metadata == "none":
        return

    # Prepare the corrected metadata frames. get_reader auto-detects the
    # backend, and the matching writer is derived from the reader type.
    src_reader = get_reader(input_files if metadata == "all" else input_files[0])
    writer_cls = Writer if isinstance(src_reader, Reader) else RNTupleWriter
    if has_metadata:
        frames = list(src_reader.get("metadata"))
    else:
        frames = [Frame()]

    for frame in frames:
        frame.put_parameter("MergedInputFiles", list(input_files))

    with tempfile.NamedTemporaryFile(suffix=".root") as tmp_file:
        tmp_file.close()
        tmp_path = tmp_file.name

        tmp_writer = writer_cls(tmp_path)
        for frame in frames:
            tmp_writer.write_frame(frame, "metadata")
        tmp_writer.finish()

        if has_metadata:
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
            _rebuild_metadata(output_file, tmp_path, writer_cls, frames)


def _main_merge(input_files, output_file, compression, has_metadata):
    """Merge all objects from all input files into the output file.

    When metadata already exists in the inputs, skip it during the main
    merge and write it correctly afterwards, to avoid duplicates.
    """
    merger = ROOT.TFileMerger(ROOT.kFALSE)
    merger.OutputFile(output_file, ROOT.kTRUE, compression)
    merger.SetFastMethod(ROOT.kTRUE)
    for fp in input_files:
        merger.AddFile(fp)
    flags = ROOT.TFileMerger.kAll
    if has_metadata:
        merger.AddObjectNames("metadata")
        flags |= ROOT.TFileMerger.kSkipListed
    if not merger.PartialMerge(flags):
        raise RuntimeError(f"TFileMerger failed merging into {output_file}")


def _first_file_compression(filename):
    """Return the compression settings of the file (like hadd -ff)."""
    first_file = ROOT.TFile.Open(filename)
    if first_file and not first_file.IsZombie():
        compression = first_file.GetCompressionSettings()
        first_file.Close()
        return compression
    return 0


def _rebuild_metadata(output_file, tmp_path, writer_cls, frames):
    """Rebuild temp file with all categories so podio_metadata schemas match.

    The RNTuple merger (and TTree merger for podio_metadata) require
    matching field/branch structures between source and target. Writing
    one frame for each category from the merged output into the temp file
    ensures the temp file's podio_metadata has all the same fields.
    """
    src_reader = get_reader(output_file)

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


def _has_metadata(filename):
    """Return True if a 'metadata' category is present in the file."""
    root_file = ROOT.TFile.Open(filename)
    if not root_file or root_file.IsZombie():
        raise RuntimeError(f"Cannot open file: {filename}")
    has_metadata = any(elem.GetName() == "metadata" for elem in root_file.GetListOfKeys())
    root_file.Close()
    return has_metadata
