#!/usr/bin/env python3
"""Utilities for python unittests"""

import os

SKIP_SIO_TESTS = os.environ.get('SKIP_SIO_TESTS', '1') == '1'
