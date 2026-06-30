#!/usr/bin/env python3
"""Utilities for merging podio ROOT files (TTree and RNTuple)."""

import shutil
import subprocess
import tempfile

import ROOT

from podio.frame import Frame
from podio.utils import convert_to_str_paths
from podio.root_io import Reader, RNTupleReader, Writer


def _hadd_path():
    """Return the path to the hadd executable, or raise if not found"""
    path = shutil.which("hadd")
    if path is None:
        raise RuntimeError(
            "hadd not found on PATH. hadd is required by podio.root_merge.merge_files "
            "and is distributed with ROOT."
        )
    return path


def merge_files(output_file, input_files, metadata="first"):
    """Merge podio ROOT files.

    Uses hadd for the event data (both TTree and RNTuple), then rewrites the
    ``metadata`` category (with the ``MergedInputFiles`` parameter set) via
    a temp file and TFileMerger.

    All input files must have been written with the same category and
    collection layout.

    Args:
        output_file (str or Path): Path of the output file to create.
        input_files (list[str] or list[Path]): Ordered list of input files.
        metadata (str): How to handle the ``metadata`` Frame category.
            ``"first"``  -- copy only the first file's entry (default).
            ``"all"``    -- copy entries from every file.
            ``"none"``   -- omit the metadata category entirely.

    Raises:
        ValueError: If *input_files* is empty or *metadata* is not one of the
            accepted values.
        RuntimeError: If a file cannot be opened, hadd is not found, or
            trees are inconsistent.
    """
    if not input_files:
        raise ValueError("input_files must not be empty")
    if metadata not in ("first", "all", "none"):
        raise ValueError(f"metadata must be 'first', 'all', or 'none', got {metadata!r}")

    input_files = [str(p) for p in convert_to_str_paths(input_files)]
    output_file = str(convert_to_str_paths(output_file)[0])

    metadata_info = _detect_metadata(input_files[0])

    hadd = _hadd_path()
    result = subprocess.run(
        [hadd, "-f", "-k", output_file] + input_files,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError(f"hadd failed (exit {result.returncode}):\n{result.stderr}")

    # Delete the incorrectly merged metadata category (rewritten below)
    # Always delete — hadd merges metadata regardless of TTree/RNTuple format
    out_f = ROOT.TFile.Open(output_file, "UPDATE")
    if not out_f or out_f.IsZombie():
        raise RuntimeError(f"Cannot open for UPDATE: {output_file}")
    try:
        out_f.Delete("metadata;*")
        out_f.Write("", ROOT.TObject.kOverwrite)
    finally:
        out_f.Close()

    if metadata == "none":
        return

    # Write the corrected metadata category via a temp file, then copy only
    # the metadata object into the output using TFileMerger
    if metadata_info is not None:
        reader_cls = metadata_info[0]
        src_reader = reader_cls([input_files[0]] if metadata == "first" else input_files)
        frames = list(src_reader.get("metadata"))
    else:
        frames = [Frame()]

    for frame in frames:
        frame.put_parameter("MergedInputFiles", list(input_files))

    with tempfile.NamedTemporaryFile(suffix=".root") as tmp_file:
        tmp_file.close()
        tmp_path = tmp_file.name

        tmp_writer = Writer(tmp_path)
        for frame in frames:
            tmp_writer.write_frame(frame, "metadata")
        tmp_writer._writer.finish()

        m = ROOT.TFileMerger(ROOT.kFALSE)
        m.OutputFile(output_file, "UPDATE")
        m.AddFile(tmp_path)
        m.SetFastMethod(ROOT.kTRUE)
        m.AddObjectNames("metadata")
        if not m.PartialMerge(
            ROOT.TFileMerger.kAll | ROOT.TFileMerger.kOnlyListed | ROOT.TFileMerger.kIncremental
        ):
            raise RuntimeError(f"TFileMerger failed adding metadata category to {output_file}")


def _detect_metadata(filename):
    """Return (reader_cls, writer_cls) if 'metadata' exists, or None.

    Checks both TTrees and RNTuples since metadata may be stored in either
    format depending on how the file was written.
    """
    f = ROOT.TFile.Open(filename)
    if not f or f.IsZombie():
        raise RuntimeError(f"Cannot open file: {filename}")
    try:
        for elem in f.GetListOfKeys():
            if elem.GetName() != "metadata":
                continue
            cls = elem.GetClassName()
            if "TTree" in cls:
                return (Reader, Writer)
            if "RNTuple" in cls:
                return (RNTupleReader, Writer)
    finally:
        f.Close()
    return None
