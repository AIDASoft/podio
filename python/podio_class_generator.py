#!/usr/bin/env python
import os
import string
import pickle
from podio_config_reader import PodioConfigReader
from podio_templates import declarations, implementations
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
    self.buildin_types = [ "int", "long", "float", "double", "unsigned int", "unsigned", "short", "bool", "longlong", "ulonglong"]
    self.created_classes = []
    self.requested_classes = []
    self.reader = PodioConfigReader(yamlfile)
    self.warnings = []

  def process(self):
    self.reader.read()
    self.process_components(self.reader.components)
    self.process_datatypes(self.reader.datatypes)
    self.create_selection_xml()
    self.print_report()

  def process_components(self,content):
    self.requested_classes += content.keys()
    for name, components in content.iteritems():
      self.create_component(name, components["Members"])

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

  def process_datatype(self, classname, definition, is_data=False):
    datatype_dict = {}
    datatype_dict["description"] = definition["Description"]
    datatype_dict["author"] = definition["Author"]
    datatype_dict["includes"] = []
    datatype_dict["members"] = []
    members = definition["Members"]
    for member in members:
      klass = member["type"]
      name = member["name"]
      description = member["description"]
      datatype_dict["members"].append("  %s %s;  ///<%s" % (klass, name, description))
      if klass in self.buildin_types:
        pass
      elif klass in self.requested_classes:
        if "::" in klass:
          namespace, klassname = klass.split("::")
          datatype_dict["includes"].append('#include "%s.h"' % klassname)
        else:
          datatype_dict["includes"].append('#include "%s.h"' % klass)
      elif "std::array" in klass:
        datatype_dict["includes"].append("#include <array>")
        self.created_classes.append(klass)
      elif "vector" in klass:
        datatype_dict["includes"].append("#include <vector>")
        if is_data:  # avoid having warnings twice
          self.warnings.append("%s defines a vector member %s, that spoils the PODness" % (classname, klass))
      elif "[" in klass and is_data:  # FIXME: is this only true ofr PODs?
        raise Exception("'%s' defines an array type. Array types are not supported yet." % (classname, klass))
      else:
        raise Exception("'%s' defines a member of a type '%s' that is not (yet) declared!" % (classname, klass))
    return datatype_dict

  def demangle_classname(self, classname):
    namespace_open = ""
    namespace_close = ""
    namespace = ""
    rawclassname = ""
    if "::" in classname:
      cnameparts = classname.split("::")
      if len(cnameparts) > 2:
        raise Exception("'%s' defines a type with nested namespaces. Not supported, yet." % classname)
      namespace, rawclassname = cnameparts
      namespace_open = "namespace %s {" % namespace
      namespace_close = "} // namespace %s" % namespace
    else:
      rawclassname = classname
    return namespace, rawclassname, namespace_open, namespace_close


  def create_data(self, classname, definition):
    # check whether all member types are known
    # and prepare include directives
    namespace, rawclassname, namespace_open, namespace_close = self.demangle_classname(classname)

    data = self.process_datatype(classname, definition)

    # now handle the vector-members
    vectormembers = definition["VectorMembers"]
    for vectormember in vectormembers:
      name = vectormember["name"]
      data["members"].append("  unsigned int %s_begin;" % name)
      data["members"].append("  unsigned int %s_end;" %(name))

    # now handle the one-to-many relations
    refvectors = definition["OneToManyRelations"]
    for refvector in refvectors:
      name = refvector["name"]
      data["members"].append("  unsigned int %s_begin;" %(name))
      data["members"].append("  unsigned int %s_end;" %(name))

    substitutions = {"includes" : "\n".join(data["includes"]),
                     "members"  : "\n".join(data["members"]),
                     "name"     : rawclassname,
                     "description" : data["description"],
                     "author"   :  data["author"],
                     "package_name" : self.package_name,
                     "namespace_open" : namespace_open,
                     "namespace_close" : namespace_close
    }
    self.fill_templates("Data", substitutions)
    self.created_classes.append(classname+"Data")

  def create_class(self, classname, definition):
    namespace, rawclassname, namespace_open, namespace_close = self.demangle_classname(classname)

    includes_cc = ""
    forward_declarations = ""
    forward_declarations_namespace = {"":[]}
    getter_implementations = ""
    setter_implementations = ""
    getter_declarations = ""
    setter_declarations = ""
    constructor_signature = ""
    constructor_body = ""
    ConstGetter_implementations = ""

    # check whether all member types are known
    # and prepare include directives
    datatype = self.process_datatype(classname, definition, False)

    datatype["includes"].append('#include "%s.h"' % (rawclassname+"Data"))

    # check on-to-one relations and prepare include directives
    oneToOneRelations = definition["OneToOneRelations"]
    for member in oneToOneRelations:
      klass = member["type"]
      if klass in self.requested_classes:
        mnamespace = ""
        klassname = klass
        if "::" in klass:
          mnamespace, klassname = klass.split("::")
          if mnamespace not in forward_declarations_namespace.keys():
            forward_declarations_namespace[mnamespace] = []

        forward_declarations_namespace[mnamespace] += ["class %s;\n" %(klassname)]
        forward_declarations_namespace[mnamespace] += ["class Const%s;\n" %(klassname)]
        includes_cc += '#include "%s.h"\n' %(klassname)

    for nsp in forward_declarations_namespace.iterkeys():
      if nsp != "":
        forward_declarations += "namespace %s {\n" % nsp
      forward_declarations += "".join(forward_declarations_namespace[nsp])
      if nsp != "":
        forward_declarations += "}\n"

    # check one-to-many relations for consistency
    # and prepare include directives
    refvectors = definition["OneToManyRelations"]
    if len(refvectors) != 0:
      datatype["includes"].append("#include <vector>")
    for item in refvectors:
      klass = item["type"]
      if klass in self.requested_classes:
        if "::" in klass:
          mnamespace, klassname = klass.split("::")
          datatype["includes"].append('#include "%s.h"' %klassname)
        else:
          datatype["includes"].append('#include "%s.h"' %klass)
      elif "std::array" in klass:
        datatype["includes"].append("#include <array>")
      else:
        raise Exception("'%s' declares a non-allowed many-relation to '%s'!" %(classname, klass))

    # handle standard members
    for member in definition["Members"]:
      name = member["name"]
      klass = member["type"]
      getter_declarations += declarations["member_getter"].format(type=klass, name=name)
      getter_implementations += implementations["member_getter"].format(type=klass, classname=rawclassname, name=name)
      if klass in self.buildin_types:
        setter_declarations += declarations["member_builtin_setter"].format(type=klass, name=name)
        setter_implementations += implementations["member_builtin_setter"].format(type=klass, classname=rawclassname, name=name)
      else:
        setter_declarations += declarations["member_class_refsetter"].format(type=klass, name=name)
        setter_implementations += implementations["member_class_refsetter"].format(type=klass, classname=rawclassname, name=name)
        setter_declarations += declarations["member_class_setter"].format(type=klass, name=name)
        setter_implementations += implementations["member_class_setter"].format(type=klass, classname=rawclassname, name=name)
      # Getter for the Const variety of this datatype
      ConstGetter_implementations += implementations["const_member_getter"].format(type=klass, classname=rawclassname, name=name)


      # set up signature
      constructor_signature += "%s %s," %(klass, name)
      # constructor
      constructor_body += "  m_obj->data.%s = %s;" %(name, name)

    # one-to-one relations
    for member in oneToOneRelations:
        name = member["name"]
        klass = member["type"]
        mnamespace = ""
        klassname = klass
        mnamespace, klassname, _, __ = self.demangle_classname(klass)

        setter_declarations += declarations["one_rel_setter"].format(name=name, namespace=mnamespace, type=klassname)
        setter_implementations += implementations["one_rel_setter"].format(name=name, namespace=mnamespace, type=klassname, classname=rawclassname)
        getter_declarations += declarations["one_rel_getter"].format(name=name, namespace=mnamespace, type=klassname)
        getter_implementations += implementations["one_rel_getter"].format(name=name, namespace=mnamespace, type=klassname, classname=rawclassname)
        ConstGetter_implementations += implementations["const_one_rel_getter"].format(name=name, namespace=mnamespace, type=klassname, classname=rawclassname)


    # handle vector members
    vectormembers = definition["VectorMembers"]
    if len(vectormembers) != 0:
      datatype["includes"].append("#include <vector>")
    for item in vectormembers:
      klass = item["type"]
      if klass not in self.buildin_types and klass not in self.reader.components:
        raise Exception("'%s' declares a non-allowed vector member of type '%s'!" %(classname, klass))
      if klass in self.reader.components:
        if "::" in klass:
          namespace, klassname = klass.split("::")
          datatype["includes"].append('#include "%s.h"' %klassname)
        else:
          datatype["includes"].append('#include "%s.h"' %klass)

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
      if relationtype not in self.buildin_types and relationtype not in self.reader.components:
          relationtype = "Const"+relationtype

      substitutions = {"relation" : refvector["name"],
                       "relationtype" : relationtype,
                       "classname" : rawclassname,
                       "package_name" : self.package_name
                      }
      references_declarations += string.Template(references_declarations_template).substitute(substitutions)
      references += string.Template(references_template).substitute(substitutions)
      references_members += "std::vector<%s>* m_%s; ///< transient \n" %(refvector["type"], refvector["name"])
      ConstReferences_declarations += string.Template(ConstReferences_declarations_template).substitute(substitutions)
      ConstReferences += string.Template(ConstReferences_template).substitute(substitutions)

    # handle user provided extra code
    extracode_declarations = ""
    extracode = ""
    constextracode_declarations = ""
    constextracode = ""
    if definition.has_key("ExtraCode"):
      extra = definition["ExtraCode"]
      extracode_declarations = extra["declaration"]
      extracode = extra["implementation"].replace("{name}",rawclassname)
      constextracode_declarations = extra["declaration"]
      constextracode = extra["implementation"].replace("{name}","Const"+rawclassname)
      # TODO: add loading of code from external files
      datatype["includes"] += extra["includes"]

    substitutions = {"includes" : "\n".join(datatype["includes"]),
                     "includes_cc" : includes_cc,
                     "forward_declarations" : forward_declarations,
                     "getters"  : getter_implementations,
                     "getter_declarations": getter_declarations,
                     "setters"  : setter_implementations,
                     "setter_declarations": setter_declarations,
                     "constructor_declaration" : constructor_declaration,
                     "constructor_implementation" : constructor_implementation,
                     "extracode" : extracode,
                     "extracode_declarations" : extracode_declarations,
                     "name"     : rawclassname,
                     "description" : datatype["description"],
                     "author"   : datatype["author"],
                     "relations" : references,
                     "relation_declarations" : references_declarations,
                     "relation_members" : references_members,
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
    substitutions["constextracode"] = constextracode
    substitutions["constextracode_declarations"] = constextracode_declarations
    self.fill_templates("ConstObject", substitutions)
    if "::" in classname:
      self.created_classes.append("%s::Const%s" %(namespace, rawclassname))
    else:
      self.created_classes.append("Const%s" %classname)

  def create_collection(self, classname, definition):
    namespace, rawclassname, namespace_open, namespace_close = self.demangle_classname(classname)

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
      constructorbody += "\tm_refCollections = new podio::CollRefCollection();\n"
      clear_relations += "\tfor (auto& pointer : (*m_refCollections)) { pointer->clear(); }\n"
      for counter, item in enumerate(refvectors):
        name  = item["name"]
        klass = item["type"]
        substitutions = { "counter" : counter,
                          "class" : klass,
                          "name"  : name }

        mnamespace, klassname, _, __ = self.demangle_classname(klass)

        # includes
        includes += '#include "%sCollection.h" \n' %(klassname)

        # FIXME check if it compiles with :: for both... then delete this.
        relations += declarations["relation"].format(namespace=mnamespace, type=klassname, name=name)
        relations += declarations["relation_collection"].format(namespace=mnamespace, type=klassname, name=name)
        initializers += implementations["ctor_list_relation"].format(namespace=mnamespace, type=klassname, name=name)

        constructorbody += "\tm_refCollections->push_back(new std::vector<podio::ObjectID>());\n"
        # relation handling in ::create
        create_relations += "\tm_rel_{name}_tmp.push_back(obj->m_{name});\n".format(name=name)
        # relation handling in ::clear
        clear_relations += implementations["clear_relations_vec"].format(name=name)
        # relation handling in push_back
        push_back_relations += "\tm_rel_{name}_tmp.push_back(obj->m_{name});\n".format(name=name)
        # relation handling in ::prepareForWrite
        prepareforwritinghead += "\tint {name}_index =0;\n".format(name=name)
        prepareforwritingbody += self.evaluate_template("CollectionPrepareForWriting.cc.template",substitutions)
        # relation handling in ::settingReferences
        setreferences += self.evaluate_template("CollectionSetReferences.cc.template",substitutions)
        prepareafterread += "\t\tobj->m_%s = m_rel_%s;" %(name, name)
      for counter, item in enumerate(refmembers):
        name  = item["name"]
        klass = item["type"]
        mnamespace = ""
        klassname = klass
        if "::" in klass:
          mnamespace, klassname = klass.split("::")
        substitutions = { "counter" : counter+nOfRefVectors,
                          "class" : klass,
                          "rawclass" : klassname,
                          "name"  : name }


        # includes
        includes += '#include "%sCollection.h"\n' %(klassname)
        # constructor call
        initializers += implementations["ctor_list_relation"].format(namespace=mnamespace, type=klassname, name=name)
        # member
        relations += declarations["relation"].format(namespace=mnamespace, type=klassname, name=name)
        constructorbody += "\tm_refCollections->push_back(new std::vector<podio::ObjectID>());\n"
        # relation handling in ::clear
        clear_relations += implementations["clear_relations"].format(name=name)
        # relation handling in ::prepareForWrite
        prepareforwriting_refmembers += implementations["prep_writing_relations"].format(name=name, i=(counter+nOfRefVectors))
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
    namespace, rawclassname, namespace_open, namespace_close = self.demangle_classname(classname)

    for klass in components.itervalues():
      if klass in self.buildin_types or klass in self.reader.components.keys():
        pass
      else:
        raise Exception("'%s' defines a member of a type '%s' which is not allowed in a component!" %(classname, klass))
    includes = ""
    members = ""
    for name, klass in components.iteritems():
      klassname = klass
      mnamespace = ""
      if "::" in klass:
        mnamespace, klassname = klass.split("::")
      if mnamespace == "":
        members+= "  %s %s;\n" %(klassname, name)
      else:
        members += " ::%s::%s %s;\n" %(mnamespace, klassname, name)
      if self.reader.components.has_key(klass):
          includes+= '#include "%s.h"\n' %(klassname)
    substitutions = { "includes" : includes,
                      "members"  : members,
                      "name"     : rawclassname,
                      "package_name" : self.package_name,
                      "namespace_open" : namespace_open,
                      "namespace_close" : namespace_close
    }
    self.fill_templates("Component",substitutions)
    self.created_classes.append(classname)

  def create_obj(self, classname, definition):
    """ Create an obj class containing all information
        relevant for a given object.
    """
    namespace, rawclassname, namespace_open, namespace_close = self.demangle_classname(classname)

    relations = ""
    includes = ""
    includes_cc = ""
    forward_declarations = ""
    forward_declarations_namespace = {"":[]}
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
      klassname = klass
      mnamespace = ""
      if "::" in klass:
        mnamespace, klassname = klass.split("::")
        if mnamespace not in forward_declarations_namespace.keys():
          forward_declarations_namespace[mnamespace] = []

      if mnamespace != "":
        relations+= "  ::%s::Const%s* m_%s;\n" %(mnamespace, klassname, name)
      else:
        relations+= "  Const%s* m_%s;\n" %(klassname, name)

      if klass not in self.buildin_types:
        if klass != classname:
          forward_declarations_namespace[mnamespace] += ['class Const%s;\n' %(klassname)]
          includes_cc += '#include "%sConst.h"\n' %(klassname)
          initialize_relations += ",m_%s(nullptr)\n" %(name)
        delete_relations+="\t\tdelete m_%s;\n" %name

    for nsp in forward_declarations_namespace.iterkeys():
      if nsp != "":
        forward_declarations += "namespace %s {" % nsp
      forward_declarations += "".join(forward_declarations_namespace[nsp])
      if nsp != "":
        forward_declarations += "}\n"

    if len(refvectors+definition["VectorMembers"]) !=0:
      includes += "#include <vector>\n"

    for item in refvectors+definition["VectorMembers"]:
      name  = item["name"]
      klass = item["type"]
      if klass not in self.buildin_types:
        if klass not in self.reader.components:
          if "::" in klass:
            mnamespace, klassname = klass.split("::")
            klassWithQualifier = "::"+mnamespace+"::Const"+klassname
          else:
            klassWithQualifier = "Const"+klass
        else:
            klassWithQualifier = klass
        relations += "\tstd::vector<%s>* m_%s;\n" %(klassWithQualifier, name)
        initialize_relations += ", m_%s(new std::vector<%s>())" %(name,klassWithQualifier)
        deepcopy_relations += ", m_%s(new std::vector<%s>(*(other.m_%s)))" %(name,klassWithQualifier,name)
        if klass == classname:
          includes_cc += '#include "%s.h"\n' %(rawclassname)
        else:
          if "::" in klass:
            mnamespace, klassname = klass.split("::")
            includes += '#include "%s.h"\n' %klassname
          else:
            includes += '#include "%s.h"\n' %klass
      else:
          relations += "\tstd::vector<%s>* m_%s;\n" %(klass, name)
          initialize_relations += ", m_%s(new std::vector<%s>())" %(name,klass)
          deepcopy_relations += ", m_%s(new std::vector<%s>(*(other.m_%s)))" %(name,klass,name)

      delete_relations += "\t\tdelete m_%s;\n" %(name)
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
      declaration += "\ttemplate<size_t arraysize>\n\tconst std::array<%s,arraysize> %s() const;\n" %(klass, name)
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
      content = string.Template(template).substitute(substitutions).expandtabs(2)
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

  #--- create output directories if they do not exist
  install_path = args[1]
  project = args[2]
  directory = os.path.join( install_path ,"src" )
  if not os.path.exists( directory ):
    os.makedirs(directory)
  directory = os.path.join( install_path , project )

  if not os.path.exists( directory ):
    os.makedirs(directory)

  gen = ClassGenerator(args[0], args[1], args[2], verbose = options.verbose)
  gen.process()
  for warning in gen.warnings:
      print warning
