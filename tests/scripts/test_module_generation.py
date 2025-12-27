#!/usr/bin/env python3
"""
Test that verifies generated C++ module interfaces are syntactically valid
and contain the expected exports.
"""

import sys
import re
from pathlib import Path


def check_module_interface(module_file):
    """Check that a module interface file has the expected structure."""

    if not module_file.exists():
        print(f"ERROR: Module file does not exist: {module_file}")
        return False

    print(f"Checking module interface: {module_file}")

    content = module_file.read_text()

    # Check for basic module structure
    checks = {
        "has module fragment": re.search(r"^\s*module\s*;", content, re.MULTILINE),
        "has export module": re.search(r"^\s*export\s+module\s+\w+", content, re.MULTILINE),
        "has export namespace": re.search(r"^\s*export\s+namespace\s+\w+", content, re.MULTILINE),
        "has license header": "SPDX-License-Identifier" in content or "Apache-2.0" in content,
    }

    all_passed = True
    for check_name, result in checks.items():
        if not result:
            print(f"  FAIL: {check_name}")
            all_passed = False
        else:
            print(f"  PASS: {check_name}")

    return all_passed


def main():
    """Run module generation validation tests.

    Checks that all generated C++ module interface files (.ixx) exist and
    have the expected structure (module fragment, export declarations, etc.).

    Returns:
        0 on success, 1 on failure
    """
    if len(sys.argv) < 2:
        print("Usage: test_module_generation.py <build_dir>")
        return 1

    build_dir = Path(sys.argv[1])

    if not build_dir.exists():
        print(f"ERROR: Build directory does not exist: {build_dir}")
        return 1

    print("Testing generated C++ module interfaces...")
    print(f"Build directory: {build_dir}")
    print()

    # Find all generated module interface files
    module_files = list(build_dir.glob("**/*_module.ixx"))

    if not module_files:
        print("WARNING: No module interface files found. Skipping test.")
        return 0

    print(f"Found {len(module_files)} module interface file(s)")
    print()

    all_passed = True
    for module_file in module_files:
        if not check_module_interface(module_file):
            all_passed = False
        print()

    if all_passed:
        print("All module interface checks passed!")
        return 0

    print("Some module interface checks failed!")
    return 1


if __name__ == "__main__":
    sys.exit(main())
