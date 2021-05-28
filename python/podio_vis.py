#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import unicode_literals, absolute_import, print_function
import os
from podio_config_reader import PodioConfigReader
from graphviz import Digraph

THIS_DIR = os.path.dirname(os.path.abspath(__file__))

class Podio2Graphviz(object):
  """Class to transform a data model description into a graphical representation"""

  def __init__(self, yamlfile, target, dot):
    self.yamlfile = yamlfile
    self.target = target
    self.use_dot = dot
    self.reader = PodioConfigReader(yamlfile)
    self.reader.read()
    self.graph = Digraph()
    self.graph.format = "png"

  def process(self):
      # first create all nodes upfront
      for name, datatype in self.reader.datatypes.items():
        compatible_name = name.replace("::","_")  # graphviz gets confused with C++ '::' and formatting strings
        self.graph.node(compatible_name, shape="box", label=name)
      # then go through the relations
      for name, datatype in self.reader.datatypes.items():
        for rel in datatype["OneToOneRelations"]:
          compatible_name = name.replace("::","_")
          compatible_type = rel.full_type.replace("::","_")
          self.graph.edge(compatible_name,compatible_type, label=rel.name)
        for rel in datatype["OneToManyRelations"]:
          compatible_name = name.replace("::","_")
          compatible_type = rel.full_type.replace("::","_")
          label = "%s (N)" %rel.name
          self.graph.edge(compatible_name,compatible_type, label=label)

      if self.use_dot:
        self.graph.save()
      else: 
        self.graph.render(self.target)

if __name__ == "__main__":
  import argparse
  parser = argparse.ArgumentParser(description='Given a description yaml file this script generates '
                                   'a visualization in the target directory')

  parser.add_argument('description', help='yaml file describing the datamodel')
  parser.add_argument('target', help='name of the file to be created')
  parser.add_argument('-d', '--dot', action='store_true', default=False,
                      help='just write the dot file')

  args = parser.parse_args()

  vis = Podio2Graphviz(args.description, args.target, args.dot)
  vis.process()
