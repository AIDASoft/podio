#!/bin/bash

echo "RUNNING PYLINT PY3K CHECK" && \
find . -name "*.py" -exec pylint --rcfile=.github/scripts/podio.pylint.py3k.rc --py3k --msg-template="{path}:{line}: [{msg_id}({symbol}), {obj}] {msg}" {} + && \
echo "" && \
echo "RUNNING FLAKE8 CHECK" && \
find . -name "*.py" -exec flake8 {} +

