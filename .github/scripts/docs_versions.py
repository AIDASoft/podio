#!/usr/bin/env python3
"""Maintain the version index of the documentation published on GitHub Pages.

The gh-pages branch contains one directory per published version (master and
all tags). This script scans these directories and (re)generates

- switcher.json: The list of all available versions in the format of the
  pydata-sphinx-theme version switcher. It is also consumed by the version
  switcher in doc/_static/version_switcher.js
- index.html: The landing page that redirects to the latest release and lists
  all available versions
"""

import argparse
import html
import json
import re
import sys
from pathlib import Path

DEV_VERSION = "master"
TAG_RE = re.compile(r"^v(\d+)-(\d+)(?:-(\d+))?$")

INDEX_TEMPLATE = """<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>PODIO documentation</title>
    <meta http-equiv="refresh" content="0; url=./{target}/index.html">
    <link rel="canonical" href="{canonical}">
    <style>
      body {{ font-family: sans-serif; max-width: 40em; margin: 2em auto; padding: 0 1em; }}
    </style>
  </head>
  <body>
    <h1>PODIO documentation</h1>
    <p>Redirecting to the documentation for
      <a href="./{target}/index.html">{target}</a>.</p>
    <h2>All versions</h2>
    <ul>
{version_list}
    </ul>
  </body>
</html>
"""


def tag_key(name):
    """Sort key for a tag name, e.g. v01-04-01 -> (1, 4, 1)"""
    match = TAG_RE.match(name)
    if match is None:
        raise ValueError(f"{name} is not a valid tag name")
    return tuple(int(group or 0) for group in match.groups())


def find_versions(site_dir):
    """Find all published versions in the site directory.

    Returns the versions with the development version first, followed by all
    tags with the newest tag first. Directories that are neither the
    development version nor a tag (e.g. leftovers from testing branches) are
    ignored, as are directories without an index.html.
    """
    candidates = [
        entry.name
        for entry in site_dir.iterdir()
        if entry.is_dir() and (entry / "index.html").is_file()
    ]
    ignored = sorted(name for name in candidates if name != DEV_VERSION and not TAG_RE.match(name))
    for name in ignored:
        print(
            f"::warning::Ignoring directory '{name}': it is neither '{DEV_VERSION}' nor a "
            "tag of the form vXX-YY[-ZZ], so it is not added to the version index",
            file=sys.stderr,
        )
    tags = sorted(
        (name for name in candidates if TAG_RE.match(name)),
        key=lambda name: (tag_key(name), name),
        reverse=True,
    )
    dev = [DEV_VERSION] if DEV_VERSION in candidates else []
    return dev + tags


def latest_tag(versions):
    """Get the latest tag from the (sorted) versions, or None if there is no tag"""
    return next((ver for ver in versions if ver != DEV_VERSION), None)


def build_switcher(versions, base_url):
    """Build the switcher.json content (pydata-sphinx-theme format)"""
    latest = latest_tag(versions)
    entries = []
    for version in versions:
        entry = {"name": version, "version": version, "url": f"{base_url}{version}/"}
        if version == DEV_VERSION:
            entry["name"] = f"{version} (development)"
        elif version == latest:
            entry["name"] = f"{version} (latest)"
            entry["preferred"] = True
        entries.append(entry)
    return json.dumps(entries, indent=2) + "\n"


def build_index(versions, base_url):
    """Build the landing page that redirects to the latest release"""
    target = latest_tag(versions) or DEV_VERSION
    version_list = "\n".join(
        f'      <li><a href="./{html.escape(ver)}/index.html">{html.escape(ver)}</a></li>'
        for ver in versions
    )
    return INDEX_TEMPLATE.format(
        target=html.escape(target),
        canonical=html.escape(f"{base_url}{target}/index.html"),
        version_list=version_list,
    )


def main():
    """Regenerate switcher.json and index.html in the site directory"""
    parser = argparse.ArgumentParser(
        description="Regenerate the version index of the published documentation"
    )
    parser.add_argument("site_dir", type=Path, help="Root of the checked out gh-pages branch")
    parser.add_argument(
        "base_url", help="Public URL of the site root, e.g. https://aidasoft.github.io/podio/"
    )
    args = parser.parse_args()
    base_url = args.base_url if args.base_url.endswith("/") else f"{args.base_url}/"

    versions = find_versions(args.site_dir)
    if not versions:
        print(f"No published versions found in {args.site_dir}", file=sys.stderr)
        return 1
    print(f"Found versions: {', '.join(versions)}")

    (args.site_dir / "switcher.json").write_text(
        build_switcher(versions, base_url), encoding="utf-8"
    )
    (args.site_dir / "index.html").write_text(build_index(versions, base_url), encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main())
