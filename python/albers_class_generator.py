#!/usr/bin/env python
import os
import string
import sys
import pickle
import yaml
thisdir = os.path.dirname(os.path.abspath(__file__))

_text_ = """






  ALBERS Data Model
  =================

  Used
    %s
  to create
    %s classes
  in
    %s/

  Read instructions in
  the HOWTO.TXT to run
  your first example!

"""

class ClassGenerator(object):

  def __init__(self,yamlfile, install_dir, verbose = True):
    self.yamlfile = yamlfile
    self.install_dir = install_dir
    self.template_dir = os.path.join(thisdir,"../templates")
    self.verbose=verbose
    self.buildin_types = ["int","float","double","unsigned int"]
    self.created_classes = []
    self.requested_classes = []

  def validate_yaml(self,content):
    pass

  def process(self):
    stream = open(self.yamlfile, "r")
    content = yaml.load(stream)
    self.validate_yaml(content)
    if content.has_key("components"):
      self.process_components(content["components"])
    if content.has_key("datatypes"):
      self.process_datatypes(content["datatypes"])
    self.create_linkDef()
    self.print_report()

  def process_components(self,content):
    """ """
    for name in content.iterkeys():
      self.requested_classes.append(name)
    for name, components in content.iteritems():
      self.create_component(name, components)

  def process_datatypes(self,content):
    for name in content.iterkeys():
      self.requested_classes.append(name)
      self.requested_classes.append("%sHandle" %name)
    for name, components in content.iteritems():
      self.create_class(name, components)
      self.create_class_handle(name, components)
      self.create_collection(name, components)

  def print_report(self):
    if self.verbose:
      pkl = open(os.path.join(thisdir,"figure.txt"))
      figure = pickle.load(pkl)
      text = _text_ % (self.yamlfile,
                       len(self.created_classes),
                       self.install_dir
                      )
      for i, line in enumerate(figure):
        print
        print line+text.splitlines()[i],
      print "     'Homage to the Square' - Josef Albers"
      print

  def create_linkDef(self):
      content = ""
      for klass in self.created_classes:
         content += "#pragma link C++ class %s+;\n" %klass
         if not (klass.endswith("Handle") or klass.endswith("Collection")):
           content += "#pragma link C++ class std::vector<%s>+;\n" %klass
      templatefile = os.path.join(self.template_dir,"LinkDef.h.template")
      template = open(templatefile,"r").read()
      content = string.Template(template).substitute({"classes" : content})
      self.write_file("LinkDef.h",content)

  def prepare_for_writing_body(self, components):
      handles = []
      for name, klass in components.iteritems():
        if klass.endswith("Handle"):
            handles.append(klass)
      prepareforwriting = ""
      if (len(handles) !=0):
        prepareforwriting = "  for(auto& data : *m_data){\n %s  }"
        handleupdate = ""
        for handle in handles:
          handleupdate+= "    data.m_%s.prepareForWrite(registry);\n" %name
        prepareforwriting= prepareforwriting % handleupdate
      return prepareforwriting
    #TODO: recursive call and support for vectors

  def prepare_after_read_body(self, components):
      handles = []
      for name, klass in components.iteritems():
        if klass.endswith("Handle"):
            handles.append(klass)
      prepareafterreadbody=""
#      prepareafterreadbody = "for(auto& data : *m_data){\n %s\n  }"
      for handle in handles:
        prepareafterreadbody+= "data.m_%s.prepareAfterRead(registry);\n" %name

      return prepareafterreadbody
    #TODO: recursive call and support for vectors

  def create_class(self, classname, components):
    # check whether all member types are known
    # and prepare include directives
    includes = ""
    for klass in components.itervalues():
      if klass in self.buildin_types:
        pass
      elif klass in self.requested_classes:
        includes += '#include "%s.h"\n' %klass
      else:
        raise Exception("'%s' defines a member of a type '%s' that is not (yet) declared!" %(classname, klass))
    members = ""
    getters = ""
    setters = ""
    for name, klass in components.iteritems():
      members+= "  %s m_%s;\n" %(klass, name)
      getters+= "  const %s& %s() const { return m_%s;};\n" %(klass, name, name)
      setters += "  void set%s(%s& value){ m_%s = value;};\n" %(name, klass, name)

    substitutions = {"includes" : includes,
                     "members"  : members,
                     "getters"  : getters,
                     "setters"  : setters,
                     "name"     : classname
    }
    self.fill_templates("POD",substitutions)
    self.created_classes.append(classname)

  def create_class_handle(self, classname, components):
    # check whether all member types are known
    # and prepare include directives
    includes = ""
    includes += '#include "%s.h"\n' %classname
    for klass in components.itervalues():
      if klass in self.buildin_types:
        pass
      elif klass in self.requested_classes:
        includes += '#include "%s.h"\n' %klass
      else:
        raise Exception("'%s' defines a member of a type '%s' that is not declared!" %(classname, klass))

    getters = ""
    setters = ""
    getter_declarations = ""
    setter_declarations = ""
    for name, klass in components.iteritems():
      getter_declarations+= "  const %s& %s() const;\n" %(klass, name)
      getters+= "  const %s& %sHandle::%s() const { return m_container->at(m_index).%s();}\n" %(klass, classname, name, name)
      setters += "  void %sHandle::set%s(%s value){ m_container->at(m_index).set%s(value);}\n" %(classname, name, klass, name)
      setter_declarations += "  void set%s(%s value);\n" %(name, klass)

    substitutions = {"includes" : includes,
                     "getters"  : getters,
                     "getter_declarations": getter_declarations,
                     "setters"  : setters,
                     "setter_declarations": setter_declarations,
                     "name"     : classname
    }
    self.fill_templates("Handle",substitutions)
    self.created_classes.append("%sHandle"%classname)

  def create_collection(self, classname, components):
    prepareforwritingbody = self.prepare_for_writing_body(components)
    prepareafterreadbody = self.prepare_after_read_body(components)
    substitutions = { "name" : classname,
                      "prepareforwritingbody" : prepareforwritingbody,
                      "prepareafterreadbody" : prepareafterreadbody
    }
    self.fill_templates("Collection",substitutions)
    self.created_classes.append("%sCollection"%classname)

  def create_component(self, classname, components):
    """ Create a component class to be used within the data types
        Components can only contain simple data types and no user 
        defined ones
    """
    for klass in components.itervalues():
      if klass in self.buildin_types:
        pass
      else:
        raise Exception("'%s' defines a member of a type '%s' which is not allowed in a component!" %(classname, klass))
    members = ""
    for name, klass in components.iteritems():
      members+= "  %s %s;\n" %(klass, name)
    substitutions = {"members"  : members,
                     "name"     : classname
    }
    self.fill_templates("Component",substitutions)
    self.created_classes.append(classname)

  def write_file(self, name,content):
    fullname = os.path.join(self.install_dir,name)
    open(fullname, "w").write(content)

  def fill_templates(self, category,substitutions):
    # "POD" denotes the real class;
    # only headers and the FN should not contain POD
    if category == "POD":
      FN = ""
      endings = ("h")
    elif category == "Component":
      FN = ""
      endings = ("h")
    else:
      FN = category
      endings = ("h","cc")
    for ending in endings:
      templatefile = "%s.%s.template" %(category,ending)
      templatefile = os.path.join(self.template_dir,templatefile)
      template = open(templatefile,"r").read()
      content = string.Template(template).substitute(substitutions)
      filename = "%s%s.%s" %(substitutions["name"],FN,ending)
      self.write_file(filename,content)


##########################
if __name__ == "__main__":

  from optparse import OptionParser


  usage = """usage: %prog [options] <description.yaml> <targetdir>

    Given a <description.yaml>
    it creates data classes
    and a LinkDef.h file in
    the specified <targetdir>.
"""
  parser = OptionParser(usage)
  parser.add_option("-q", "--quiet",
                    action="store_false", dest="verbose", default=True,
                    help="Don't write a report to screen")
  (options, args) = parser.parse_args()

  if len(args) != 2:
      parser.error("incorrect number of arguments")

  gen = ClassGenerator(args[0], args[1], verbose = options.verbose)
  gen.process()
