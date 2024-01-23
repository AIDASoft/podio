#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Podio class generator script"""

import os
import subprocess


from podio_gen.podio_config_reader import PodioConfigReader
from podio_gen.generator_utils import DefinitionError
from podio_gen.cpp_generator import CPPClassGenerator
from podio_gen.julia_generator import JuliaClassGenerator


def has_clang_format():
    """Check if clang format is available"""
    try:
        # This one can raise if -fallback-style is not found
        out = subprocess.check_output(
            ["clang-format", "-style=file", "-fallback-style=llvm", "--help"],
            stderr=subprocess.STDOUT,
        )
        # This one doesn't raise
        out = subprocess.check_output(
            "echo | clang-format -style=file ", stderr=subprocess.STDOUT, shell=True
        )
        if b".clang-format" in out:
            return False
        return True
    except FileNotFoundError:
        print("ERROR: Cannot find clang-format executable")
        print("       Please make sure it is in the PATH.")
        return False
    except subprocess.CalledProcessError:
        print("ERROR: At least one argument was not recognized by clang-format")
        print("       Most likely the version you are using is old")
        return False


def clang_format_file(content, name):
    """Formatter function to run clang-format on generate c++ files"""
    if name.endswith(".jl"):
        return content

    clang_format = ["clang-format", "-style=file", "-fallback-style=llvm"]
    with subprocess.Popen(clang_format, stdin=subprocess.PIPE, stdout=subprocess.PIPE) as cfproc:
        return cfproc.communicate(input=content.encode())[0].decode()


def read_upstream_edm(name_path):
    """Read an upstream EDM yaml definition file to make the types that are defined
    in that available to the current EDM"""
    if name_path is None:
        return None

    try:
        name, path = name_path.split(":")
    except ValueError as err:
        raise argparse.ArgumentTypeError(
            "upstream-edm argument needs to be the upstream package "
            "name and the upstream edm yaml file separated by a colon"
        ) from err

    if not os.path.isfile(path):
        raise argparse.ArgumentTypeError(f"{path} needs to be an EDM yaml file")

    try:
        return PodioConfigReader.read(path, name)
    except DefinitionError as err:
        raise argparse.ArgumentTypeError(
            f"{path} does not contain a valid datamodel definition"
        ) from err


if __name__ == "__main__":
    import argparse

    # pylint: disable=invalid-name # before 2.5.0 pylint is too strict with the naming here
    parser = argparse.ArgumentParser(
        description="Given a description yaml file this script generates "
        "the necessary c++ or julia files in the target directory"
    )

    parser.add_argument("description", help="yaml file describing the datamodel")
    parser.add_argument(
        "targetdir",
        help="Target directory where the generated data classes will be put. "
        "Header files will be put under <targetdir>/<packagename>/*.h. "
        "Source files will be put under <targetdir>/src/*.cc. "
        "Julia files will be put under <targetdir>/<packagename>/*.jl.",
    )
    parser.add_argument("packagename", help="Name of the package.")
    parser.add_argument(
        "iohandlers",
        choices=["ROOT", "SIO"],
        nargs="*",
        help="The IO backend specific code that should be generated",
        default="ROOT",
    )
    parser.add_argument(
        "-l",
        "--lang",
        choices=["cpp", "julia"],
        default="cpp",
        help="Specify the programming language (default: cpp)",
    )
    parser.add_argument(
        "-q",
        "--quiet",
        dest="verbose",
        action="store_false",
        default=True,
        help="Don't write a report to screen",
    )
    parser.add_argument(
        "-d",
        "--dryrun",
        action="store_true",
        default=False,
        help="Do not actually write datamodel files",
    )
    parser.add_argument(
        "-c",
        "--clangformat",
        action="store_true",
        default=False,
        help="Apply clang-format when generating code (with -style=file)",
    )
    parser.add_argument(
        "--upstream-edm",
        help="Make datatypes of this upstream EDM available to the current"
        " EDM. Format is '<upstream-name>:<upstream.yaml>'. "
        "Note that only the code for the current EDM will be generated",
        default=None,
        type=read_upstream_edm,
    )
    parser.add_argument(
        "--old-description",
        help="Provide schema evolution relative to the old yaml file.",
        default=None,
        action="store",
    )
    parser.add_argument(
        "-e",
        "--evolution_file",
        help="yaml file clarifying schema evolutions",
        default=None,
        action="store",
    )

    args = parser.parse_args()

    install_path = args.targetdir
    project = args.packagename

    for sub_dir in ("src", project):
        directory = os.path.join(install_path, sub_dir)
        if not os.path.exists(directory):
            os.makedirs(directory)

    if args.lang == "julia":
        gen = JuliaClassGenerator(
            args.description,
            args.targetdir,
            args.packagename,
            verbose=args.verbose,
            dryrun=args.dryrun,
            upstream_edm=args.upstream_edm,
        )
    if args.lang == "cpp":
        gen = CPPClassGenerator(
            args.description,
            args.targetdir,
            args.packagename,
            args.iohandlers,
            verbose=args.verbose,
            dryrun=args.dryrun,
            upstream_edm=args.upstream_edm,
            old_description=args.old_description,
            evolution_file=args.evolution_file,
        )

        if args.clangformat and has_clang_format():
            gen.formatter_func = clang_format_file

    gen.process()

    # pylint: enable=invalid-name
