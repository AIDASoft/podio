#!/usr/bin/env python
import os
import string
import sys
import pickle
import yaml
from podio_config_reader import PodioConfigReader
thisdir = os.path.dirname(os.path.abspath(__file__))

_text_ = """






  PODIO Data Model
  ================

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
    self.buildin_types = ["int","float","double","unsigned int","unsigned","short","bool","longlong","ulonglong"]
    self.created_classes = []
    self.requested_classes = []
    self.reader = PodioConfigReader(yamlfile)
    self.warnings = []

  def load_data_definitions(self):
    self.datatypes = self.reader.read()

  def process(self):
    stream = open(self.yamlfile, "r")
    content = yaml.load(stream)
    if content.has_key("components"):
      self.components = content["components"]
      self.process_components(content["components"])
    self.load_data_definitions()
    self.process_datatypes(self.datatypes)
    self.create_selection_xml()
    self.print_report()

  def process_components(self,content):
    for name in content.iterkeys():
      self.requested_classes.append(name)
    for name, components in content.iteritems():
      self.create_component(name, components)

  def process_datatypes(self,content):
    for name in content.iterkeys():
      self.requested_classes.append(name)
      self.requested_classes.append("%sData" %name)
    for name, components in content.iteritems():
      self.create_data(name, components)
      self.create_class(name, components)
      self.create_collection(name, components)
      self.create_obj(name, components)

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

  def get_template(self,filename):
      templatefile = os.path.join(self.template_dir,filename)
      return open(templatefile,"r").read()

  def create_selection_xml(self):
      content = ""
      for klass in self.created_classes:
        #if not klass.endswith("Collection") or klass.endswith("Data"):
          content += '          <class name="std::vector<%s>" />\n' %klass
          content += """
          <class name="%s">
            <field name="m_registry" transient="true"/>
            <field name="m_container" transient="true"/>
          </class>\n""" %klass

      templatefile = os.path.join(self.template_dir,"selection.xml.template")
      template = open(templatefile,"r").read()
      content = string.Template(template).substitute({"classes" : content})
      self.write_file("selection.xml",content)

  def create_data(self, classname, definition):
    # check whether all member types are known
    # and prepare include directives
    namespace_open = ""
    namespace_close = ""
    if "::" in classname:
      namespace, rawclassname = classname.split("::")
      namespace_open = "namespace %s {" % namespace
      namespace_close = "} // namespace %s" % namespace
    else:
      rawclassname = classname

    includes = ""
    description = definition["Description"]
    author      = definition["Author"]
    members = definition["Members"]
    for member in members:
      klass = member["type"]
      name  = member["type"]
      if klass in self.buildin_types:
        pass
      elif klass in self.requested_classes:
    #    includes += '#include "%s/%s.h"\n' %(self.package_name,klass)
        includes += '#include "%s.h"\n' %(klass)
      elif "std::array" in klass:
          includes += "#include <array>\n"
          self.created_classes.append(klass)
      elif "vector" in klass:
          includes += "#include <vector>\n"
          self.warnings.append("%s defines a vector member %s, that spoils the PODness" %(classname, klass))
      elif "[" in klass:
          raise Exception("'%s' defines an array type. Array types are not supported yet." %(classname, klass))
      else:
        raise Exception("'%s' defines a member of a type '%s' that is not (yet) declared!" %(classname, klass))
    membersCode = ""
    for member in members:
      name = member["name"]
      klass = member["type"]
      description = member["description"]
      membersCode+= "  %s %s; ///%s \n" %(klass, name, description)

    # now handle the vector-members
    vectormembers = definition["VectorMembers"]
    for vectormember in vectormembers:
      name = vectormember["name"]
      membersCode+= "  unsigned int %s_begin; \n" %(name)
      membersCode+= "  unsigned int %s_end; \n" %(name)

    # now handle the one-to-many relations
    refvectors = definition["OneToManyRelations"]
    for refvector in refvectors:
      name = refvector["name"]
      membersCode+= "  unsigned int %s_begin; \n" %(name)
      membersCode+= "  unsigned int %s_end; \n" %(name)

    substitutions = {"includes" : includes,
                     "members"  : membersCode,
                     "name"     : rawclassname,
                     "description" : description,
                     "author"   : author,
                     "package_name" : self.package_name,
                     "namespace_open" : namespace_open,
                     "namespace_close" : namespace_close
    }
    self.fill_templates("Data",substitutions)
    self.created_classes.append(classname+"Data")

  def create_class(self, classname, definition):
    namespace_open = ""
    namespace_close = ""
    if "::" in classname:
      namespace, rawclassname = classname.split("::")
      namespace_open = "namespace %s {" % namespace
      namespace_close = "} // namespace %s" % namespace
    else:
      rawclassname = classname
    includes = ""
    includes_cc = ""
    forward_declarations = ""
    getter_implementations = ""
    setter_implementations = ""
    getter_declarations = ""
    setter_declarations = ""
    constructor_signature = ""
    constructor_body = ""
    ConstGetter_implementations = ""

    # check whether all member types are known
    # and prepare include directives
    includes += '#include "%s.h"\n' %(rawclassname+"Data")
    #includes += '#include "%s/%s.h"\n' %(self.package_name,classname+"Data")
    description = definition["Description"]
    author      = definition["Author"]
    members = definition["Members"]
    for member in members:
      klass = member["type"]
      if klass in self.buildin_types:
        pass
      elif klass in self.requested_classes:
        #includes += '#include "%s/%s.h"\n' %(self.package_name,klass)
        includes += '#include "%s.h"\n' %(klass)
      elif "std::array" in klass:
        includes += "#include <array>\n"
      elif "vector" in klass:
        includes += "#include <vector>\n"
      else:
        raise Exception("'%s' defines a member of a type '%s' that is not declared!" %(classname, klass))

    # check on-to-one relations and prepare include directives
    oneToOneRelations = definition["OneToOneRelations"]
    for member in oneToOneRelations:
      klass = member["type"]
      if klass in self.requested_classes:
        forward_declarations += "class %s;\n" %(klass)
        forward_declarations += "class Const%s;\n" %(klass)
        includes_cc += '#include "%s.h"\n' %(klass)

    # check one-to-many relations for consistency
    # and prepare include directives
    refvectors = definition["OneToManyRelations"]
    if len(refvectors) != 0:
      includes += "#include <vector>\n"
    for item in refvectors:
      klass = item["type"]
      if klass in self.requested_classes:
        includes += '#include "%s.h"\n' %klass
      elif "std::array" in klass:
        includes += "#include <array>\n"
      else:
        raise Exception("'%s' declares a non-allowed many-relation to '%s'!" %(classname, klass))

    # handle standard members
    for member in members:
      name = member["name"]
      klass = member["type"]
      description = member["description"]
      getter_declarations+= "  const %s& %s() const;\n" %(klass, name)
      getter_implementations+= "  const %s& %s::%s() const { return m_obj->data.%s; }\n" %(klass,rawclassname,name, name)
#      getter_declarations+= "  %s& %s() { return m_obj->data.%s; };\n" %(klass, name, name)
      if klass in self.buildin_types:
        setter_declarations += "  void %s(%s value);\n" %(name, klass)
        setter_implementations += "void %s::%s(%s value){ m_obj->data.%s = value;}\n" %(rawclassname, name, klass, name)
      else:
        setter_declarations += "  %s& %s();\n" %(klass, name)  # getting non-const reference is conceptually a setter
        setter_implementations += "  %s& %s::%s() { return m_obj->data.%s; }\n" %(klass, rawclassname,name, name)
        setter_declarations += "  void %s(class %s value);\n" %(name, klass)
        setter_implementations += "void %s::%s(class %s value){ m_obj->data.%s = value;}\n" %(rawclassname, name, klass, name)
      # set up signature
      constructor_signature += "%s %s," %(klass, name)
      # constructor
      constructor_body += "  m_obj->data.%s = %s;" %(name, name)
      ConstGetter_implementations += "  const %s& Const%s::%s() const { return m_obj->data.%s; }\n" %(klass, rawclassname, name, name)

    # one-to-one relations
    for member in oneToOneRelations:
        name = member["name"]
        klass = member["type"]
        setter_declarations += "  void %s(Const%s value);\n" %(name, klass)
        setter_implementations += "void %s::%s(Const%s value) { if (m_obj->m_%s != nullptr) delete m_obj->m_%s; m_obj->m_%s = new Const%s(value); }\n" %(rawclassname,name, klass, name, name, name,klass)
        getter_declarations += "  const Const%s %s() const;\n" %(klass, name)
        getter_implementations += "  const Const%s %s::%s() const { if (m_obj->m_%s == nullptr) {\n return Const%s(nullptr);}\n return Const%s(*(m_obj->m_%s));}\n" \
                                  %(klass, rawclassname, name, name, klass, klass, name)
        ConstGetter_implementations += "  const Const%s Const%s::%s() const { if (m_obj->m_%s == nullptr) {\n return Const%s(nullptr);}\n return Const%s(*(m_obj->m_%s));}\n" \
                                  %(klass, rawclassname, name, name, klass, klass, name)

    # handle vector members
    vectormembers = definition["VectorMembers"]
    if len(vectormembers) != 0:
      includes += "#include <vector>\n"
    for item in vectormembers:
      klass = item["type"]
      if klass not in self.buildin_types and klass not in self.components:
        raise Exception("'%s' declares a non-allowed vector member of type '%s'!" %(classname, klass))
      if klass in self.components:
        includes += '#include "%s.h"\n' %klass

    # handle constructor from values
    constructor_signature = constructor_signature.rstrip(",")
    if constructor_signature == "":
       constructor_implementation = ""
       constructor_declaration = ""
       ConstConstructor_declaration = ""
       ConstConstructor_implementation = ""
    else:
      substitutions = { "name" : rawclassname,
                        "constructor" : constructor_body,
                        "signature" : constructor_signature
      }
      constructor_implementation = self.evaluate_template("Object.constructor.cc.template",substitutions)
      constructor_declaration = "  %s(%s);\n" %(rawclassname, constructor_signature)
      ConstConstructor_implementation = self.evaluate_template("ConstObject.constructor.cc.template",substitutions)
      ConstConstructor_declaration = "Const%s(%s);\n" %(rawclassname, constructor_signature)

    # handle one-to-many relations
    references_members = ""
    references_declarations = ""
    references = ""
    ConstReferences_declarations = ""
    ConstReferences = ""
    references_template = self.get_template("RefVector.cc.template")
    references_declarations_template = self.get_template("RefVector.h.template")
    ConstReferences_declarations_template = self.get_template("ConstRefVector.h.template")
    ConstReferences_template = self.get_template("ConstRefVector.cc.template")

    for refvector in refvectors+definition["VectorMembers"]:
      relationtype = refvector["type"]
      if relationtype not in self.buildin_types and relationtype not in self.components:
          relationtype = "Const"+relationtype

      substitutions = {"relation" : refvector["name"],
                       "relationtype" : relationtype,
                       "classname" : rawclassname,
                       "package_name" : self.package_name
                      }
      references_declarations += string.Template(references_declarations_template).substitute(substitutions)
      references += string.Template(references_template).substitute(substitutions)
      references_members += "std::vector<%s>* m_%s; //! transient \n" %(refvector["type"], refvector["name"])
      ConstReferences_declarations += string.Template(ConstReferences_declarations_template).substitute(substitutions)
      ConstReferences += string.Template(ConstReferences_template).substitute(substitutions)


    substitutions = {"includes" : includes,
                     "includes_cc" : includes_cc,
                     "forward_declarations" : forward_declarations,
                     "getters"  : getter_implementations,
                     "getter_declarations": getter_declarations,
                     "setters"  : setter_implementations,
                     "setter_declarations": setter_declarations,
                     "constructor_declaration" : constructor_declaration,
                     "constructor_implementation" : constructor_implementation,
                     "name"     : rawclassname,
                     "description" : description,
                     "author"   : author,
                     "relations" : references,
                     "relation_declarations" : references_declarations,
                     "relation_members" : references_members,
                     ""
                     "package_name" : self.package_name,
                     "namespace_open" : namespace_open,
                     "namespace_close" : namespace_close
                    }
    self.fill_templates("Object",substitutions)
    self.created_classes.append(classname)


    substitutions["constructor_declaration"] = ConstConstructor_declaration
    substitutions["constructor_implementation"] = ConstConstructor_implementation
    substitutions["relation_declarations"] = ConstReferences_declarations
    substitutions["relations"] = ConstReferences
    substitutions["getters"]   = ConstGetter_implementations
    self.fill_templates("ConstObject", substitutions)
    if "::" in classname:
      self.created_classes.append("%s::Const%s" %(namespace, rawclassname))
    else:
      self.created_classes.append("Const%s" %classname)

  def create_collection(self, classname, definition):
    namespace_open = ""
    namespace_close = ""
    if "::" in classname:
      namespace, rawclassname = classname.split("::")
      namespace_open = "namespace %s {" % namespace
      namespace_close = "} // namespace %s" % namespace
    else:
      rawclassname = classname
    members = definition["Members"]
    constructorbody = ""
    prepareforwritinghead = ""
    prepareforwritingbody = ""
    vectorized_access_decl, vectorized_access_impl = self.prepare_vectorized_access(rawclassname,members)
    setreferences = ""
    prepareafterread = ""
    includes = ""
    initializers = ""
    relations = ""
    create_relations = ""
    clear_relations  = ""
    push_back_relations = ""
    prepareafterread_refmembers = ""
    prepareforwriting_refmembers = ""

    refmembers = definition["OneToOneRelations"]
    refvectors = definition["OneToManyRelations"]
    nOfRefVectors = len(refvectors)
    nOfRefMembers = len(refmembers)
    if nOfRefVectors + nOfRefMembers > 0:
      # member initialization
      constructorbody += "  m_refCollections = new podio::CollRefCollection();\n"
      clear_relations += "  for (auto& pointer : (*m_refCollections)) {pointer->clear(); }\n"
      for counter, item in enumerate(refvectors):
        name  = item["name"]
        klass = item["type"]
        substitutions = { "counter" : counter,
                          "class" : klass,
                          "name"  : name }
        # includes
        includes += '#include "%sCollection.h" \n' %(klass)
        # members
        relations += "  std::vector<Const%s>* m_rel_%s; //relation buffer for r/w\n" %(klass, name)
        relations += "  std::vector<std::vector<Const%s>*> m_rel_%s_tmp;\n " %(klass, name)
        # constructor calls
        initializers += ",m_rel_%s(new std::vector<Const%s>())" %(name, klass)
        constructorbody += "  m_refCollections->push_back(new std::vector<podio::ObjectID>());\n"
        # relation handling in ::create
        create_relations += "  m_rel_%s_tmp.push_back(obj->m_%s);\n" %(name,name)
        # relation handling in ::clear
        clear_relations += "  // clear relations to %s. Make sure to unlink() the reference data as they may be gone already\n" %(name)
        clear_relations += "  for (auto& pointer : m_rel_%s_tmp) {for(auto& item : (*pointer)) {item.unlink();}; delete pointer;}\n" %(name)
        clear_relations += "  m_rel_%s_tmp.clear();\n" %(name)
        clear_relations += "  for (auto& item : (*m_rel_%s)) {item.unlink(); }\n" %(name)
        clear_relations += "  m_rel_%s->clear();\n" %(name)
        # relation handling in push_back
        push_back_relations += "  m_rel_%s_tmp.push_back(obj->m_%s);\n" %(name,name)
        # relation handling in ::prepareForWrite
        prepareforwritingbody += self.evaluate_template("CollectionPrepareForWriting.cc.template",substitutions)
        # relation handling in ::settingReferences
        setreferences += self.evaluate_template("CollectionSetReferences.cc.template",substitutions)
        prepareafterread += "    obj->m_%s = m_rel_%s;" %(name, name)
      for counter, item in enumerate(refmembers):
        name  = item["name"]
        klass = item["type"]
        substitutions = { "counter" : counter+nOfRefVectors,
                          "class" : klass,
                          "name"  : name }
        # includes
        includes += '#include "%sCollection.h" \n' %(klass)
        # members
        relations += "  std::vector<Const%s>* m_rel_%s; //relation buffer for r/w\n" %(klass, name)
        # constructor calls
        initializers += ",m_rel_%s(new std::vector<Const%s>())" %(name, klass)
        constructorbody += "  m_refCollections->push_back(new std::vector<podio::ObjectID>());\n"
        # relation handling in ::clear
        clear_relations += "  for (auto& item : (*m_rel_%s)) {item.unlink(); }\n" %(name)
        clear_relations += "  m_rel_%s->clear();\n" %(name)
        # relation handling in ::prepareForWrite
        prepareforwriting_refmembers +=  "  for (auto& obj : m_entries) {\nif (obj->m_%s != nullptr){\n(*m_refCollections)[%s]->emplace_back(obj->m_%s->getObjectID());} else {(*m_refCollections)[%s]->push_back({-2,-2}); } }\n" %(name,counter+nOfRefVectors,name,counter+nOfRefVectors)
        # relation handling in ::settingReferences
        prepareafterread_refmembers += self.evaluate_template("CollectionSetSingleReference.cc.template",substitutions)
    substitutions = { "name" : rawclassname,
                      "constructorbody" : constructorbody,
                      "prepareforwritinghead" : prepareforwritinghead,
                      "prepareforwritingbody" : prepareforwritingbody,
                      "prepareforwriting_refmembers" : prepareforwriting_refmembers,
                      "setreferences" : setreferences,
                      "prepareafterread" : prepareafterread,
                      "prepareafterread_refmembers" : prepareafterread_refmembers,
                      "includes" : includes,
                      "initializers" : initializers,
                      "relations"           : relations,
                      "create_relations" : create_relations,
                      "clear_relations"  : clear_relations,
                      "push_back_relations" : push_back_relations,
                      "package_name" : self.package_name,
                      "vectorized_access_declaration" : vectorized_access_decl,
                      "vectorized_access_implementation" : vectorized_access_impl,
                      "namespace_open" : namespace_open,
                      "namespace_close" : namespace_close
    }
    self.fill_templates("Collection",substitutions)
    self.created_classes.append("%sCollection"%classname)

  def create_component(self, classname, components):
    """ Create a component class to be used within the data types
        Components can only contain simple data types and no user
        defined ones
    """
    for klass in components.itervalues():
      if klass in self.buildin_types or self.components.has_key(klass):
        pass
      else:
        raise Exception("'%s' defines a member of a type '%s' which is not allowed in a component!" %(classname, klass))
    includes = ""
    members = ""
    for name, klass in components.iteritems():
      members+= "  %s %s;\n" %(klass, name)
      if self.components.has_key(klass):
          includes+= '#include "%s.h"\n' %(klass)
    substitutions = { "includes" : includes,
                      "members"  : members,
                      "name"     : classname,
                      "package_name" : self.package_name
    }
    self.fill_templates("Component",substitutions)
    self.created_classes.append(classname)

  def create_obj(self, classname, definition):
    """ Create an obj class containing all information
        relevant for a given object.
    """
    namespace_open = ""
    namespace_close = ""
    if "::" in classname:
      namespace, rawclassname = classname.split("::")
      namespace_open = "namespace %s {" % namespace
      namespace_close = "} // namespace %s" % namespace
    else:
      rawclassname = classname
    relations = ""
    includes = ""
    includes_cc = ""
    forward_declarations = ""
    initialize_relations = ""
    deepcopy_relations = ""
    delete_relations = ""
    refvectors = definition["OneToManyRelations"]
    singleRelations = definition["OneToOneRelations"]
    # do includes and forward declarations for
    # oneToOneRelations and do proper cleanups
    for item in singleRelations:
      name  = item["name"]
      klass = item["type"]
      relations+= "  Const%s* m_%s;\n" %(klass, name)
      if klass not in self.buildin_types:
        if klass != classname:
          forward_declarations += 'class Const%s;\n' %(klass)
          includes_cc += '#include "%sConst.h"\n' %(klass)
          initialize_relations += ",m_%s(nullptr)\n" %(name)
        delete_relations+="delete m_%s;\n" %name

    if len(refvectors+definition["VectorMembers"]) !=0:
      includes += "#include <vector>\n"

    for item in refvectors+definition["VectorMembers"]:
      name  = item["name"]
      klass = item["type"]
      if klass not in self.buildin_types:
        if klass not in self.components:
            klassWithQualifier = "Const"+klass
        else:
            klassWithQualifier = klass
        relations += "  std::vector<%s>* m_%s;\n" %(klassWithQualifier, name)
        initialize_relations += ",m_%s(new std::vector<%s>())" %(name,klassWithQualifier)
        deepcopy_relations += ",m_%s(new std::vector<%s>(*(other.m_%s)))" %(name,klassWithQualifier,name)
        if klass == classname:
          includes_cc += '#include "%s.h"\n' %(rawclassname)
        else:
          includes += '#include "%s.h"\n' %(klass)
      else:
          relations += "  std::vector<%s>* m_%s;\n" %(klass, name)
          initialize_relations += ",m_%s(new std::vector<%s>())" %(name,klass)
          deepcopy_relations += ",m_%s(new std::vector<%s>(*(other.m_%s)))" %(name,klass,name)

      delete_relations += "delete m_%s;\n" %(name)
    substitutions = { "name" : rawclassname,
                      "includes" : includes,
                      "includes_cc" : includes_cc,
                      "forward_declarations" : forward_declarations,
                      "relations" : relations,
                      "initialize_relations" : initialize_relations,
                      "deepcopy_relations" : deepcopy_relations,
                      "delete_relations" : delete_relations,
                      "namespace_open" : namespace_open,
                      "namespace_close" : namespace_close
    }
    self.fill_templates("Obj",substitutions)
    self.created_classes.append(classname+"Obj")

  def prepare_vectorized_access(self, classname,members ):
    implementation = ""
    declaration = ""
    for member in members:
      name = member["name"]
      klass = member["type"]
      substitutions = { "classname" : classname,
                       "member"    : name,
                       "type"      : klass
                       }
      if klass not in self.buildin_types:
        substitutions["type"] = "class %s" % klass
      implementation += self.evaluate_template("CollectionReturnArray.cc.template", substitutions)
      declaration += "  template<size_t arraysize>  \n  const std::array<%s,arraysize> %s() const;\n" %(klass, name)
    return declaration, implementation

  def write_file(self, name,content):
    #dispatch headers to header dir, the rest to /src
    # fullname = os.path.join(self.install_dir,self.package_name,name)
    if name.endswith("h"):
     fullname = os.path.join(self.install_dir,self.package_name,name)
    else:
     fullname = os.path.join(self.install_dir,"src",name)
    open(fullname, "w").write(content)

  def evaluate_template(self, filename, substitutions):
      """ reads in a given template, evaluates it
          and returns result
      """
      templatefile = os.path.join(self.template_dir,filename)
      template = open(templatefile,"r").read()
      return string.Template(template).substitute(substitutions)

  def fill_templates(self, category, substitutions):
    # "Data" denotes the real class;
    # only headers and the FN should not contain Data
    if category == "Data":
      FN = "Data"
      endings = ("h")
    elif category == "Obj":
      FN = "Obj"
      endings = ("h","cc")
    elif category == "Component":
      FN = ""
      endings = ("h")
    elif category == "Object":
      FN = ""
      endings = ("h","cc")
    elif category == "ConstObject":
      FN = "Const"
      endings = ("h","cc")
    else:
      FN = category
      endings = ("h","cc")
    for ending in endings:
      templatefile = "%s.%s.template" %(category,ending)
      templatefile = os.path.join(self.template_dir,templatefile)
      template = open(templatefile,"r").read()
      content = string.Template(template).substitute(substitutions)
      filename = "%s%s.%s" %(substitutions["name"],FN,ending)
      self.write_file(filename, content)


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
  for warning in gen.warnings:
      print warning
