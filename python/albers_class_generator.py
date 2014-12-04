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

  def __init__(self,yamlfile, install_dir, package_name, verbose = True):
    self.yamlfile = yamlfile
    self.install_dir = install_dir
    self.package_name =package_name
    self.template_dir = os.path.join(thisdir,"../templates")
    self.verbose=verbose
    self.buildin_types = ["int","float","double","unsigned int","unsigned"]
    self.created_classes = []
    self.requested_classes = []

  def process(self):
    stream = open(self.yamlfile, "r")
    content = yaml.load(stream)
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

  def prepare_for_writing_body(self, members):
      handles = []
      for member in members:
        name = member["name"]
        klass = member["type"]
        if klass.endswith("Handle"):
            handles.append(name)
      prepareforwriting = ""
      if (len(handles) !=0):
        prepareforwriting = "  for(auto& data : *m_data){\n %s  }"
        handleupdate = ""
        for handle in handles:
          handleupdate+= "    data.%s.prepareForWrite(registry);\n" %handle
        prepareforwriting= prepareforwriting % handleupdate
      return prepareforwriting
    #TODO: recursive call and support for vectors

  def prepare_after_read_body(self, members):
      handles = []
      for member in members:
        name = member["name"]
        klass = member["type"]
        if klass.endswith("Handle"):
            handles.append(name)
      prepareafterreadbody=""
#      prepareafterreadbody = "for(auto& data : *m_data){\n %s\n  }"
      for handle in handles:
        prepareafterreadbody+= "data.%s.prepareAfterRead(registry);\n" %handle

      return prepareafterreadbody
    #TODO: recursive call and support for vectors

  def create_class(self, classname, definition):
    # check whether all member types are known
    # and prepare include directives
    includes = ""
    description = definition["description"]
    author      = definition["author"]
    members = definition["members"]
    for member in members:
      klass = member["type"]
      if klass in self.buildin_types:
        pass
      elif klass in self.requested_classes:
        includes += '#include "%s/%s.h"\n' %(self.package_name,klass)
      else:
        raise Exception("'%s' defines a member of a type '%s' that is not (yet) declared!" %(classname, klass))
    membersCode = ""
#    gettersCode = ""
#    settersCode = ""
    for member in members:
      name = member["name"]
      klass = member["type"]
      description = member["description"]
      membersCode+= "  %s %s; //%s \n" %(klass, name, description)

    # now handle the one-to-many relations
    refvectors = []
    if definition.has_key("OneToManyRelations"):
      refvectors = definition["OneToManyRelations"]
    for refvector in refvectors:
      name = refvector["name"]
      membersCode+= "  unsigned int %s_begin; \n" %(name)
      membersCode+= "  unsigned %s_end; \n" %(name)    

    substitutions = {"includes" : includes,
                     "members"  : membersCode,
                     "name"     : classname,
                     "description" : description,
                     "author"   : author,
                     "package_name" : self.package_name 
    }
    self.fill_templates("POD",substitutions)
    self.created_classes.append(classname)

  def create_class_handle(self, classname, definition):
    # check whether all member types are known
    # and prepare include directives
    includes = ""
    includes += '#include "%s/%s.h"\n' %(self.package_name,classname)
    description = definition["description"]
    author      = definition["author"]
    members = definition["members"]
    for member in members:
      klass = member["type"]
      if klass in self.buildin_types:
        pass
      elif klass in self.requested_classes:
        includes += '#include "%s/%s.h"\n' %(self.package_name,klass)
      else:
        raise Exception("'%s' defines a member of a type '%s' that is not declared!" %(classname, klass))

    # check one-to-many relations for consistency
    # and prepare include directives
    refvectors = []
    if definition.has_key("OneToManyRelations"):
      includes += "#include <vector>\n"
      refvectors = definition["OneToManyRelations"]
      for item in refvectors:
        klass = item["type"]
        if klass in self.requested_classes:
          includes += '#include "%sHandle.h"\n' %klass
        else:
          raise Exception("'%s' declares a non-allowed many-relation to '%s'!" %(classname\
, klass))

    getters = ""
    setters = ""
    getter_declarations = ""
    setter_declarations = ""

    # handle standard members
    for member in members:
      name = member["name"]
      klass = member["type"]
      description = member["description"]
      getter_declarations+= "  const %s& %s() const;\n" %(klass, name)
      getters+= "  const %s& %sHandle::%s() const { return m_container->at(m_index).%s;}\n" %(klass, classname, name, name)
      setters += "  void %sHandle::set%s(%s value){ m_container->at(m_index).%s = value;}\n" %(classname, name, klass, name)
      setter_declarations += "  void set%s(%s value);\n" %(name, klass)

    # handle one-to-many relations
    references_members = ""
    references_declarations = ""
    references = "" 
    templatefile = os.path.join(self.template_dir,"RefVector.cc.template")
    references_template = open(templatefile,"r").read()
    templatefile = os.path.join(self.template_dir,"RefVector.h.template")
    references_declarations_template = open(templatefile,"r").read() 
    for refvector in refvectors:
      substitutions = {"relation" : refvector["name"],
                       "relationtype" : refvector["type"], 
                       "classname" : classname,
                       "package_name" : self.package_name
                      }
      references_declarations += string.Template(references_declarations_template).substitute(substitutions)
      references += string.Template(references_template).substitute(substitutions)
      references_members += "std::vector<%sHandle>* m_%s; //! transient \n" %(refvector["type"], refvector["name"])

    substitutions = {"includes" : includes,
#                     "getters"  : getters,
#                     "getter_declarations": getter_declarations,
#                     "setters"  : setters,
#                     "setter_declarations": setter_declarations,
                     "name"     : classname,
                     "description" : description,
                     "author"   : author,
                     "relations" : references,
                     "relation_declarations" : references_declarations,
                     "relation_members" : references_members,
                     "package_name" : self.package_name
                    }
    self.fill_templates("Handle",substitutions)
    self.created_classes.append("%sHandle"%classname)

  def create_collection(self, classname, definition):
    members = definition["members"]
    prepareforwritingbody = self.prepare_for_writing_body(members)
    prepareafterreadbody = self.prepare_after_read_body(members)
    # handle one-to-many-relations
    relations = ""
    create_relations = ""
    clear_relations  = ""
    if definition.has_key("OneToManyRelations"):
      refvectors = definition["OneToManyRelations"]
      for item in refvectors:
        name  = item["name"]
        klass = item["type"] 
        # members
        relations += "std::vector<%sHandle>* m_rel_%s; //relation buffer for r/w\n" %(klass, name)
        relations += "  std::vector<std::vector<%sHandle>*> m_rel_%s_tmp;\n " %(klass, name)
        # relation handling in ::create
        create_relations += "  auto %s_tmp = new std::vector<%sHandle>();\n" %(name, klass)
        create_relations += "  m_rel_%s_tmp.push_back(%s_tmp);\n" %(name,name)
        create_relations += "  tmp_handle.m_%s = %s_tmp;\n" %(name, name) 
        # relation handling in ::clear
        clear_relations += "  for (auto& pointer : m_rel_%s_tmp) {delete pointer;}\n" %(name)
        clear_relations += "  m_rel_%s_tmp.clear();\n" %(name)
        clear_relations += "  m_rel_%s->clear();\n" %(name) 
        # relation handling in ::prepareForWrite

        # relation handling in ::prepareAfterRead

    substitutions = { "name" : classname,
                      "prepareforwritingbody" : prepareforwritingbody,
                      "prepareafterreadbody" : prepareafterreadbody,
                      "relations"           : relations,
                      "create_relations" : create_relations,
                      "clear_relations"  : clear_relations,
                      "package_name" : self.package_name
    }
    self.fill_templates("Collection",substitutions)
    self.created_classes.append("%sCollection"%classname)

  def create_component(self, classname, components):
    """ Create a component class to be used within the data types
        Components can only contain simple data types and no user 
        defined ones
    """
    includes = ""
    for klass in components.itervalues():
      if klass in self.buildin_types:
        pass
      elif klass in self.requested_classes:
        includes += '#include "%s/%s.h"\n' %(self.package_name,klass)
      else:
        raise Exception("'%s' defines a member of a type '%s' which is not allowed in a component!" %(classname, klass))
    members = ""
    for name, klass in components.iteritems():
      members+= "  %s %s;\n" %(klass, name)
    substitutions = { "includes" : includes,
                      "members"  : members,
                      "name"     : classname,
                      "package_name" : self.package_name
                      }
    self.fill_templates("Component",substitutions)
    self.created_classes.append(classname)

  def write_file(self, name,content):
    #dispatch headers to header dir, the rest to /src
    if name.endswith("h"):
      fullname = os.path.join(self.install_dir,self.package_name,name)
    else:
      fullname = os.path.join(self.install_dir,"src",name)
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


class YAMLValidator(object):
  pass

##########################
if __name__ == "__main__":

  from optparse import OptionParser


  usage = """usage: %prog [options] <description.yaml> <targetdir> <packagename>

    Given a <description.yaml>
    it creates data classes
    and a LinkDef.h file in
    the specified <targetdir>:
      <packagename>/*.h
      src/*.cc
"""
  parser = OptionParser(usage)
  parser.add_option("-q", "--quiet",
                    action="store_false", dest="verbose", default=True,
                    help="Don't write a report to screen")
  (options, args) = parser.parse_args()

  if len(args) != 3:
      parser.error("incorrect number of arguments")

  gen = ClassGenerator(args[0], args[1], args[2], verbose = options.verbose)
  gen.process()
