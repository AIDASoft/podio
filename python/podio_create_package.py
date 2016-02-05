#!/usr/bin/env python

# This script creates a data model package according to the following
# structure:
#
# CMakeLists.txt
# definitions/
#
#




##########################
if __name__ == "__main__":
  from optparse import OptionParser

  usage = "usage: %prog " %TODO
  parser = OptionParser(usage)
  parser.add_option("-q", "--quiet",
                    action="store_false", dest="verbose", default=True,
                    help="Don't write a report to screen")
  (options, args) = parser.parse_args()
