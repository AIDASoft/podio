#!/usr/bin/env python3
"""Utilities for merging podio ROOT files (TTree and RNTuple)."""

import os
import shutil
import subprocess
import tempfile

import ROOT

from podio.frame import Frame
from podio.utils import convert_to_str_paths
from podio.root_io import Reader, RNTupleReader, RNTupleWriter, Writer


def _get_tree_names(filename):
    """Return the names of all TTrees in a ROOT file"""
    f = ROOT.TFile.Open(filename)
    if not f or f.IsZombie():
        raise RuntimeError(f"Cannot open file: {filename}")
    names = {elem.GetName() for elem in f.GetListOfKeys() if elem.GetClassName() == "TTree"}
    f.Close()
    return names


def _get_rntuple_names(filename):
    """Return the names of all RNTuples in a ROOT file"""
    f = ROOT.TFile.Open(filename)
    if not f or f.IsZombie():
        raise RuntimeError(f"Cannot open file: {filename}")
    names = {elem.GetName() for elem in f.GetListOfKeys() if elem.GetClassName() == "ROOT::RNTuple"}
    f.Close()
    return names


def _hadd_path():
    """Return the path to the hadd executable, or raise if not found"""
    path = shutil.which("hadd")
    if path is None:
        raise RuntimeError(
            "hadd not found on PATH. hadd is required by podio.root_merge.merge_files "
            "and is distributed with ROOT."
        )
    return path


def _merge_files_impl(output_file, input_files, metadata, get_names_fn, reader_cls, writer_cls, fmt_name):
    """Common implementation for merge_files and merge_files_rntuple.

    Args:
        output_file (str): Path of the output file to create.
        input_files (list[str]): Ordered list of input files (already str).
        metadata (str): ``"first"``, ``"all"``, or ``"none"``.
        get_names_fn: Callable returning the set of object names in a file.
        reader_cls: Reader class to use (Reader or RNTupleReader).
        writer_cls: Writer class to use (Writer or RNTupleWriter).
        fmt_name (str): Format label used in error messages (e.g. "TTree").
    """
    has_metadata_cat = "metadata" in get_names_fn(input_files[0])

    hadd = _hadd_path()
    result = subprocess.run([hadd, "-f", "-k", output_file] + input_files, capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"hadd failed (exit {result.returncode}):\n{result.stderr}")

    # Delete the incorrectly merged metadata category (rewritten below)
    out_f = ROOT.TFile.Open(output_file, "UPDATE")
    if not out_f or out_f.IsZombie():
        raise RuntimeError(f"Cannot open for UPDATE: {output_file}")
    try:
        if has_metadata_cat:
            out_f.Delete("metadata;*")
        out_f.Write("", ROOT.TObject.kOverwrite)
    finally:
        out_f.Close()

    if metadata == "none":
        return

    # Write the corrected metadata category via a temp file, then copy only
    # the metadata object into the output using TFileMerger
    if has_metadata_cat:
        src_reader = reader_cls([input_files[0]] if metadata == "first" else input_files)
        frames = list(src_reader.get("metadata"))
    else:
        frames = [Frame()]

    for frame in frames:
        frame.put_parameter("MergedInputFiles", list(input_files))

    tmp_fd, tmp_path = tempfile.mkstemp(suffix=".root")
    os.close(tmp_fd)
    try:
        tmp_writer = writer_cls(tmp_path)
        for frame in frames:
            tmp_writer.write_frame(frame, "metadata")
        tmp_writer._writer.finish()

        m = ROOT.TFileMerger(ROOT.kFALSE)
        m.OutputFile(output_file, "UPDATE")
        m.AddFile(tmp_path)
        m.SetFastMethod(ROOT.kTRUE)
        m.AddObjectNames("metadata")
        if not m.PartialMerge(ROOT.TFileMerger.kAll | ROOT.TFileMerger.kOnlyListed | ROOT.TFileMerger.kIncremental):
            raise RuntimeError(f"TFileMerger failed adding metadata {fmt_name} to {output_file}")
    finally:
        if os.path.exists(tmp_path):
            os.unlink(tmp_path)


def merge_files(output_file, input_files, metadata="first"):
    """Merge podio TTree files.

    Uses hadd for the event data, then rewrites the ``metadata`` category
    (with the ``MergedInputFiles`` parameter set) via a temp file
    and TFileMerger.

    All input files must have been written with the same category and
    collection layout.

    Args:
        output_file (str or Path): Path of the output file to create.
        input_files (list[str] or list[Path]): Ordered list of input files.
        metadata (str): How to handle the ``metadata`` Frame category.
            ``"first"``  – copy only the first file's entry (default).
            ``"all"``    – copy entries from every file.
            ``"none"``   – omit the metadata category entirely.

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

    _merge_files_impl(output_file, input_files, metadata, _get_tree_names, Reader, Writer, "TTree")


def merge_files_rntuple(output_file, input_files, metadata="first"):
    """Merge podio RNTuple files.

    Uses hadd for the event data, then rewrites the ``metadata`` category
    (with the ``MergedInputFiles`` parameter set) via a temp file
    and TFileMerger.

    All input files must have been written with the same category and
    collection layout.

    Args:
        output_file (str or Path): Path of the output file to create.
        input_files (list[str] or list[Path]): Ordered list of input files.
        metadata (str): How to handle the ``metadata`` Frame category.
            ``"first"``  – copy only the first file's entry (default).
            ``"all"``    – copy entries from every file.
            ``"none"``   – omit the metadata category entirely.

    Raises:
        ValueError: If *input_files* is empty or *metadata* is not one of the
            accepted values.
        RuntimeError: If a file cannot be opened, hadd is not found, or
            ntuples are inconsistent.
    """
    if not input_files:
        raise ValueError("input_files must not be empty")
    if metadata not in ("first", "all", "none"):
        raise ValueError(f"metadata must be 'first', 'all', or 'none', got {metadata!r}")

    input_files = [str(p) for p in convert_to_str_paths(input_files)]
    output_file = str(convert_to_str_paths(output_file)[0])

    _merge_files_impl(output_file, input_files, metadata, _get_rntuple_names, RNTupleReader, RNTupleWriter, "RNTuple")
