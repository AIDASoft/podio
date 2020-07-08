#!/usr/bin/env python

from __future__ import absolute_import, unicode_literals, print_function
from io import open
try:
  from itertools import zip_longest
except ImportError:
  from itertools import izip_longest as zip_longest

from collections import OrderedDict
from copy import deepcopy

import os
import string
import pickle
import subprocess
from podio_config_reader import PodioConfigReader, ClassDefinitionValidator
from podio_templates import declarations, implementations
thisdir = os.path.dirname(os.path.abspath(__file__))

REPORT_TEXT = """
  PODIO Data Model
  ================
  Used
    {yamlfile}
  to create
    {nclasses} classes
  in
    {installdir}/
  Read instructions in
  the HOWTO.TXT to run
  your first example!
"""

def demangle_classname(classname):
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


def get_clang_format():
  """Check if clang format is available and if so get the list of arguments to
  invoke it via subprocess.Popen"""
  try:
    cformat_exe = subprocess.check_output(['which', 'clang-format']).strip()
    return [cformat_exe, "-style=file", "-fallback-style=llvm"]
  except subprocess.CalledProcessError:
    print("ERROR: Cannot find clang-format executable")
    print("       Please make sure it is in the PATH.")
    return []


def ostream_component(comp_members, classname, osname='o', valname='value'):
  """Define the inline ostream operator for component structs"""
  ostream = ['inline std::ostream& operator<<( std::ostream& {o}, const {classname}& {value} ) {{']

  for klass, name in comp_members:
    arr_match = ClassDefinitionValidator.array_re.search(klass)
    if arr_match:
      size = arr_match.group(2)
      ostream.append('  for (int i = 0; i < {size}; ++i) {{{{'.format(size=size))
      ostream.append('    {{o}} << {{value}}.{name}[i] << "|";'.format(name=name))
      ostream.append('  }}')
      ostream.append('  {o} << " ";')
    else:
      ostream.append('  {{o}} << {{value}}.{name} << " ";'.format(name=name))

  ostream.append('  return {o};')
  ostream.append('}}\n')
  return '\n'.join(ostream).format(
    o=osname, classname=classname, value=valname
  )


class ClassGenerator(object):

  def __init__(self, yamlfile, install_dir, package_name,
               verbose=True, dryrun=False):

    self.yamlfile = yamlfile
    self.install_dir = install_dir
    self.package_name = package_name
    self.template_dir = os.path.join(thisdir, "templates")
    self.verbose = verbose
    self.buildin_types = ClassDefinitionValidator.buildin_types
    self.created_classes = []
    self.requested_classes = []
    self.warnings = set()
    self.component_members = OrderedDict()
    self.dryrun = dryrun

    self.reader = PodioConfigReader(yamlfile)
    self.reader.read()
    self.get_syntax = self.reader.options["getSyntax"]
    self.include_subfolder = self.reader.options["includeSubfolder"]
    self.expose_pod_members = self.reader.options["exposePODMembers"]

    self.clang_format = []


  def process(self):
    self.process_components(self.reader.components)
    self.process_datatypes(self.reader.datatypes)
    self.create_selection_xml()
    self.print_report()

  def process_components(self, content):
    self.requested_classes += content.keys()
    for name, components in content.items():
      self.create_component(name, components["Members"])

  def process_datatypes(self, content):
    # First have to fill the requested classes before the actual generation can
    # start
    for name in content:
      self.requested_classes.append(name)
      self.requested_classes.append("%sData" % name)

    for name, components in content.items():
      datatype = self._process_datatype(name, components)

      self.create_data(name, components, datatype)
      self.create_class(name, components, datatype)
      self.create_collection(name, components)
      self.create_obj(name, components)
      # self.create_PrintInfo(name, components)


  def print_report(self):
    if not self.verbose:
      return

    text = REPORT_TEXT.format(yamlfile=self.yamlfile,
                              nclasses=len(self.created_classes),
                              installdir=self.install_dir)

    with open(os.path.join(thisdir, "figure.txt"), 'rb') as pkl:
      figure = pickle.load(pkl)

    print()
    for figline, summaryline in zip_longest(figure, text.splitlines(), fillvalue=''):
      print(figline + summaryline)
    print("     'Homage to the Square' - Josef Albers")
    print()

    for warning in self.warnings:
      print(warning)


  def get_template(self, filename):
    templatefile = os.path.join(self.template_dir, filename)
    with open(templatefile, 'r') as tempfile:
      return tempfile.read()


  def create_selection_xml(self):
    content = ""
    for klass in self.created_classes:
      # if not klass.endswith("Collection") or klass.endswith("Data"):
      content += '          <class name="std::vector<%s>" />\n' % klass
      content += """
            <class name="%s">
              <field name="m_registry" transient="true"/>
              <field name="m_container" transient="true"/>
            </class>\n""" % klass

    templatefile = os.path.join(self.template_dir,
                                "selection.xml.template")
    template = self.get_template(templatefile)
    content = string.Template(template).substitute({"classes": content})
    self.write_file("selection.xml", content)


  def _process_datatype(self, classname, definition):
    """Check whether all members are known and prepare include directories"""
    datatype_dict = {
      "description": definition["Description"],
      "author": definition["Author"],
      "includes": set(),
      "members": []
    }
    for member in definition["Members"]:
      klass = member["type"]
      name = member["name"]
      description = member["description"]
      datatype_dict["members"].append("  %s %s;  ///< %s"
                                      % (klass, name, description))

      array_match = ClassDefinitionValidator.array_re.search(klass)
      if array_match:
        datatype_dict["includes"].add("#include <array>")
        array_type = array_match.group(1)
        if array_type not in self.buildin_types:
          if "::" in array_type:
            array_type = array_type.split("::")[1]
          datatype_dict["includes"].add(self._build_include(array_type))

      elif "std::string" == klass:
        datatype_dict["includes"].add("#include <string>")
        self.warnings.add("%s defines a string member %s, that spoils the PODness"
                          % (classname, klass))

      elif klass in self.requested_classes:
        if "::" in klass:
          _, klassname = klass.split("::")
          datatype_dict["includes"].add(self._build_include(klassname))
        else:
          datatype_dict["includes"].add(self._build_include(klass))

      elif "vector" in klass:
        datatype_dict["includes"].add("#include <vector>")
        self.warnings.add("%s defines a vector member %s, that spoils the PODness" % (classname, klass))
      elif "[" in klass:
        raise Exception("'%s' defines an array type %s. Array types are not supported yet." %
                        (classname, klass))

      elif klass in self.buildin_types:
        pass
      else:
        raise Exception("'%s' defines a member of a type '%s' that is not (yet) declared!" % (classname, klass))
    # get rid of duplicates:
    return datatype_dict


  def create_data(self, classname, definition, datatype):
    data = deepcopy(datatype) # avoid having outside side-effects
    # now handle the vector-members
    vectormembers = definition["VectorMembers"]
    for vectormember in vectormembers:
      name = vectormember["name"]
      data["members"].append("  unsigned int %s_begin;" % name)
      data["members"].append("  unsigned int %s_end;" % (name))

    # now handle the one-to-many relations
    refvectors = definition["OneToManyRelations"]
    for refvector in refvectors:
      name = refvector["name"]
      data["members"].append("  unsigned int %s_begin;" % (name))
      data["members"].append("  unsigned int %s_end;" % (name))

    _, rawclassname, namespace_open, namespace_close = demangle_classname(classname)
    substitutions = {"includes": self._join_set(data["includes"]),
                     "members": "\n".join(data["members"]),
                     "name": rawclassname,
                     "description": data["description"],
                     "author": data["author"],
                     "package_name": self.package_name,
                     "namespace_open": namespace_open,
                     "namespace_close": namespace_close
                     }
    self.fill_templates("Data", substitutions)
    self.created_classes.append(classname + "Data")


  def create_class(self, classname, definition, datatype):
    datatype = deepcopy(datatype) # avoid having outside side-effects
    namespace, rawclassname, namespace_open, namespace_close = demangle_classname(classname)

    includes_cc = set()
    forward_declarations = ""
    forward_declarations_namespace = {"": []}
    getter_implementations = ""
    setter_implementations = ""
    getter_declarations = ""
    setter_declarations = ""
    constructor_signature = ""
    constructor_body = ""
    ConstGetter_implementations = ""
    ostream_declaration = ""
    ostream_implementation = ""

    datatype["includes"].add(self._build_include(rawclassname + "Data"))

    ostream_declaration = ("std::ostream& operator<<( std::ostream& o,const Const%s& value );\n" % rawclassname)
    ostream_implementation = ("std::ostream& operator<<( std::ostream& o,const Const%s& value ){\n" % rawclassname)
    ostream_implementation += '  o << " id : " << value.id() << std::endl ;\n'

    # check on-to-one relations and prepare include directives
    oneToOneRelations = definition["OneToOneRelations"]
    for member in oneToOneRelations:
      klass = member["type"]
      if klass in self.requested_classes:
        mnamespace = ""
        klassname = klass
        if "::" in klass:
          mnamespace, klassname = klass.split("::")
          if mnamespace not in forward_declarations_namespace:
            forward_declarations_namespace[mnamespace] = []

        forward_declarations_namespace[mnamespace] += ["class %s;\n" % (klassname)]
        forward_declarations_namespace[mnamespace] += ["class Const%s;\n" % (klassname)]
        includes_cc.add(self._build_include(klassname))

    for nsp in forward_declarations_namespace:
      if nsp != "":
        forward_declarations += "namespace %s {\n" % nsp
      forward_declarations += "".join(forward_declarations_namespace[nsp])
      if nsp != "":
        forward_declarations += "}\n"

    # check one-to-many relations for consistency
    # and prepare include directives
    refvectors = definition["OneToManyRelations"]
    if len(refvectors) != 0:
      datatype["includes"].add("#include <vector>")
      datatype["includes"].add('#include "podio/RelationRange.h"')
    for item in refvectors:
      klass = item["type"]
      if klass in self.requested_classes:
        if "::" in klass:
          mnamespace, klassname = klass.split("::")
          datatype["includes"].add(self._build_include(klassname))
        else:
          datatype["includes"].add(self._build_include(klass))
      elif "std::array" in klass:
        datatype["includes"].add("#include <array>")
        array_type = klass.split("<")[1].split(",")[0]
        if array_type not in self.buildin_types:
          if "::" in array_type:
            array_type = array_type.split("::")[1]
          datatype["includes"].add(self._build_include(array_type))
      else:
        raise Exception("'%s' declares a non-allowed many-relation to '%s'!" % (classname, klass))

    # handle standard members
    all_members = OrderedDict()
    for member in definition["Members"]:
      name = member["name"]
      klass = member["type"]
      desc = member["description"]
      gname, sname = name, name
      if(self.get_syntax):
        gname = "get" + name[:1].upper() + name[1:]
        sname = "set" + name[:1].upper() + name[1:]
      if name in all_members:
        raise Exception("'%s' clashes with another member name in class '%s', previously defined in %s" %
                        (name, classname, all_members[name]))
      all_members[name] = classname

      getter_declarations += declarations["member_getter"].format(
          type=klass, name=name, fname=gname, description=desc)
      getter_implementations += implementations["member_getter"].format(
          type=klass, classname=rawclassname, name=name, fname=gname)
      if klass in self.buildin_types:
        setter_declarations += declarations["member_builtin_setter"].format(
            type=klass, name=name, fname=sname, description=desc)
        setter_implementations += implementations["member_builtin_setter"].format(
            type=klass, classname=rawclassname, name=name, fname=sname)
        ostream_implementation += ('  o << " %s : " << value.%s() << std::endl ;\n' % (name, gname))
      elif klass.startswith("std::array"):
        setter_declarations += declarations["member_builtin_setter"].format(
            type=klass, name=name, fname=sname, description=desc)
        setter_implementations += implementations["member_builtin_setter"].format(
            type=klass, classname=rawclassname, name=name, fname=sname)
        item_class = klass.split("<")[1].split(",")[0].strip()
        setter_declarations += declarations["array_builtin_setter"].format(
            type=item_class, name=name, fname=sname, description=desc)
        setter_implementations += implementations["array_builtin_setter"].format(
            type=item_class, classname=rawclassname, name=name, fname=sname)
        getter_declarations += declarations["array_member_getter"].format(
            type=item_class, name=name, fname=gname, description=desc)
        getter_implementations += implementations["array_member_getter"].format(
            type=item_class, classname=rawclassname, name=name, fname=gname)
        ConstGetter_implementations += implementations["const_array_member_getter"].format(
            type=item_class, classname=rawclassname, name=name, fname=gname, description=desc)
        arrsize = klass[klass.rfind(',') + 1: klass.rfind('>')]
        ostream_implementation += ('  o << " %s : " ;\n' % (name))
        ostream_implementation += '  for(int i=0,N=' + arrsize + ';i<N;++i)\n'
        ostream_implementation += ('      o << value.%s()[i] << "|" ;\n' % gname)
        ostream_implementation += '  o << std::endl ;\n'
      else:
        ostream_implementation += ('  o << " %s : " << value.%s() << std::endl ;\n' % (name, gname))
        setter_declarations += declarations["member_class_refsetter"].format(
            type=klass, name=name, description=desc)
        setter_implementations += implementations["member_class_refsetter"].format(
            type=klass, classname=rawclassname, name=name, fname=sname)
        setter_declarations += declarations["member_class_setter"].format(
            type=klass, name=name, fname=sname, description=desc)
        setter_implementations += implementations["member_class_setter"].format(
            type=klass, classname=rawclassname, name=name, fname=sname)
        if self.expose_pod_members:
          sub_members = self.component_members[klass]
          for sub_member in sub_members:
            comp_member_class, comp_member_name = sub_member
            if comp_member_name in all_members:
              raise Exception("'%s' clashes with another member name in class '%s'"
                              "(defined in the component '%s' and '%s')" % (
                                  comp_member_name, classname, name, all_members[comp_member_name]))
            all_members[comp_member_name] = " member '" + name + "'"
            # use mystructMember with camel case as name to avoid clashes
            comp_gname, comp_sname = comp_member_name, comp_member_name
            if self.get_syntax:
              comp_gname = "get" + comp_member_name[:1].upper() + comp_member_name[1:]
              comp_sname = "set" + comp_member_name[:1].upper() + comp_member_name[1:]

            ostream_implementation += ('  o << " %s : " << value.%s() << std::endl ;\n' %
                                       (comp_member_name, comp_gname))

            getter_declarations += declarations["pod_member_getter"].format(
                type=comp_member_class, name=comp_member_name, fname=comp_gname,
                compname=name, description=desc)
            getter_implementations += implementations["pod_member_getter"].format(
                type=comp_member_class, classname=rawclassname, name=comp_member_name, fname=comp_gname,
                compname=name, description=desc)
            if comp_member_class in self.buildin_types:
              setter_declarations += declarations["pod_member_builtin_setter"].format(
                  type=comp_member_class, name=comp_member_name, fname=comp_sname,
                  compname=name, description=desc)
              setter_implementations += implementations["pod_member_builtin_setter"].format(
                  type=comp_member_class, classname=rawclassname, name=comp_member_name, fname=comp_sname,
                  compname=name, description=desc)
            else:
              setter_declarations += declarations["pod_member_class_refsetter"].format(
                  type=comp_member_class, name=comp_member_name, compname=name, description=desc)
              setter_implementations += implementations["pod_member_class_refsetter"].format(
                  type=comp_member_class, classname=rawclassname, name=comp_member_name, fname=comp_sname,
                  compname=name, description=desc)
              setter_declarations += declarations["pod_member_class_setter"].format(
                  type=comp_member_class, name=comp_member_name, fname=comp_sname,
                  compname=name, description=desc)
              setter_implementations += implementations["pod_member_class_setter"].format(
                  type=comp_member_class, classname=rawclassname, fname=comp_sname, name=comp_member_name,
                  compname=name, description=desc)
            ConstGetter_implementations += implementations["const_pod_member_getter"].format(
                type=comp_member_class, classname=rawclassname, name=comp_member_name, fname=comp_gname,
                compname=name, description=desc)

      # Getter for the Const variety of this datatype
      ConstGetter_implementations += implementations["const_member_getter"].format(
          type=klass, classname=rawclassname, name=name, fname=gname, description=desc)

      # set up signature
      constructor_signature += "%s %s," % (klass, name)
      # constructor
      constructor_body += "  m_obj->data.%s = %s;" % (name, name)

    # one-to-one relations
    for member in oneToOneRelations:
      name = member["name"]
      klass = member["type"]
      desc = member["description"]
      mnamespace = ""
      klassname = klass
      mnamespace, klassname, _, __ = demangle_classname(klass)

      gname = name
      sname = name
      if self.get_syntax:
        gname = "get" + name[:1].upper() + name[1:]
        sname = "set" + name[:1].upper() + name[1:]
      ostream_implementation += ('  o << " %s : " << value.%s().id() << std::endl ;\n' % (name, gname))

      setter_declarations += declarations["one_rel_setter"].format(
          name=name, fname=sname, namespace=mnamespace, type=klassname, description=desc)
      setter_implementations += implementations["one_rel_setter"].format(
          name=name, fname=sname, namespace=mnamespace, type=klassname, classname=rawclassname)

      getter_declarations += declarations["one_rel_getter"].format(
          name=name, fname=gname, namespace=mnamespace, type=klassname, description=desc)
      getter_implementations += implementations["one_rel_getter"].format(
          name=name, fname=gname, namespace=mnamespace, type=klassname, classname=rawclassname)
      ConstGetter_implementations += implementations["const_one_rel_getter"].format(
          name=name, fname=gname, namespace=mnamespace, type=klassname, classname=rawclassname, description=desc)

    # handle vector members
    vectormembers = definition["VectorMembers"]
    if len(vectormembers) != 0:
      datatype["includes"].add("#include <vector>")
      datatype["includes"].add('#include "podio/RelationRange.h"')
    for item in vectormembers:
      klass = item["type"]
      if klass not in self.buildin_types and klass not in self.reader.components:
        raise Exception("'%s' declares a non-allowed vector member of type '%s'!" % (classname, klass))
      if klass in self.reader.components:
        if "::" in klass:
          namespace, klassname = klass.split("::")
          datatype["includes"].add(self._build_include(klassname))
        else:
          datatype["includes"].add(self._build_include(klass))

    # handle constructor from values
    constructor_signature = constructor_signature.rstrip(",")
    if constructor_signature == "":
      constructor_implementation = ""
      constructor_declaration = ""
      ConstConstructor_declaration = ""
      ConstConstructor_implementation = ""
    else:
      substitutions = {"name": rawclassname,
                       "constructor": constructor_body,
                       "signature": constructor_signature
                       }
      constructor_implementation = self.evaluate_template("Object.constructor.cc.template", substitutions)
      constructor_declaration = "  %s(%s);\n" % (rawclassname, constructor_signature)
      ConstConstructor_implementation = self.evaluate_template(
          "ConstObject.constructor.cc.template", substitutions)
      ConstConstructor_declaration = "Const%s(%s);\n" % (rawclassname, constructor_signature)

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

    for refvector in refvectors + definition["VectorMembers"]:
      relnamespace, reltype, _, __ = demangle_classname(refvector["type"])
      relationtype = refvector["type"]
      if relationtype not in self.buildin_types and relationtype not in self.reader.components:
        relationtype = relnamespace
        if relnamespace:
          relationtype += "::"
        relationtype += "Const" + reltype

      relationName = refvector["name"]
      get_relation = relationName
      add_relation = "add" + relationName

      if(self.get_syntax):
        get_relation = "get" + relationName[:1].upper() + relationName[1:]
        add_relation = "addTo" + relationName[:1].upper() + relationName[1:]

      ostream_implementation += ('  o << " %s : " ;\n' % relationName)
      ostream_implementation += ('  for(unsigned i=0,N=value.%s_size(); i<N ; ++i)\n' % relationName)
      # If the reference is of the same type as the class we have to avoid
      # running into a possible infinite loop when printing. Hence, print only
      # the id instead of the whole referenced object
      if reltype == classname:
        ostream_implementation += ('    o << value.%s(i).id() << " " ; \n' % get_relation)
      else:
        ostream_implementation += ('    o << value.%s(i) << " " ; \n' % get_relation)
      ostream_implementation += '  o << std::endl ;\n'

      substitutions = {"relation": relationName,
                       "get_relation": get_relation,
                       "add_relation": add_relation,
                       "relationtype": relationtype,
                       "classname": rawclassname,
                       "package_name": self.package_name
                       }
      references_declarations += string.Template(references_declarations_template).substitute(substitutions)
      references += string.Template(references_template).substitute(substitutions)
      references_members += "std::vector<%s>* m_%s; ///< transient \n" % (refvector["type"], refvector["name"])
      ConstReferences_declarations += string.Template(
          ConstReferences_declarations_template).substitute(substitutions)
      ConstReferences += string.Template(ConstReferences_template).substitute(substitutions)

    # handle user provided extra code
    extracode_declarations = ""
    extracode = ""
    constextracode_declarations = ""
    constextracode = ""
    if "ExtraCode" in definition:
      extra = definition["ExtraCode"]
      if "declaration" in extra:
        extracode_declarations = extra["declaration"].replace("{name}", rawclassname)
      if "implementation" in extra:
        extracode = extra["implementation"].replace("{name}", rawclassname)
      if "const_declaration" in extra:
        constextracode_declarations = extra["const_declaration"].replace("{name}", "Const" + rawclassname)
        extracode_declarations += "\n"
        extracode_declarations += extra["const_declaration"]
      if "const_implementation" in extra:
        constextracode = extra["const_implementation"].replace("{name}", "Const" + rawclassname)
        extracode += "\n"
        extracode += extra["const_implementation"].replace("{name}", rawclassname)
      # TODO: add loading of code from external files
      if "includes" in extra:
        extraIncludes = set(extra["includes"].split('\n'))
        datatype["includes"].update(extraIncludes)

    ostream_implementation += "  return o ;\n}\n"

    substitutions = {"includes": self._join_set(datatype["includes"]),
                     "includes_cc": self._join_set(includes_cc),
                     "forward_declarations": forward_declarations,
                     "getters": getter_implementations,
                     "getter_declarations": getter_declarations,
                     "setters": setter_implementations,
                     "setter_declarations": setter_declarations,
                     "constructor_declaration": constructor_declaration,
                     "constructor_implementation": constructor_implementation,
                     "extracode": extracode,
                     "extracode_declarations": extracode_declarations,
                     "name": rawclassname,
                     "description": datatype["description"],
                     "author": datatype["author"],
                     "relations": references,
                     "relation_declarations": references_declarations,
                     "relation_members": references_members,
                     "package_name": self.package_name,
                     "namespace_open": namespace_open,
                     "namespace_close": namespace_close,
                     "ostream_declaration": ostream_declaration,
                     "ostream_implementation": ostream_implementation
                     }
    self.fill_templates("Object", substitutions)
    self.created_classes.append(classname)

    substitutions["constructor_declaration"] = ConstConstructor_declaration
    substitutions["constructor_implementation"] = ConstConstructor_implementation
    substitutions["relation_declarations"] = ConstReferences_declarations
    substitutions["relations"] = ConstReferences
    substitutions["getters"] = ConstGetter_implementations
    substitutions["constextracode"] = constextracode
    substitutions["constextracode_declarations"] = constextracode_declarations
    self.fill_templates("ConstObject", substitutions)
    if "::" in classname:
      self.created_classes.append("%s::Const%s" % (namespace, rawclassname))
    else:
      self.created_classes.append("Const%s" % classname)

  def create_collection(self, classname, definition):
    namespace, rawclassname, namespace_open, namespace_close = demangle_classname(classname)

    members = definition["Members"]
    constructorbody = ""
    destructorbody = ""
    prepareforwritinghead = ""
    prepareforwritingbody = ""
    vectorized_access_decl, vectorized_access_impl = self.prepare_vectorized_access(rawclassname, members)
    setreferences = ""
    prepareafterread = ""
    includes = set()
    initializers = ""
    relations = ""
    create_relations = ""
    clear_relations = ""
    push_back_relations = ""
    prepareafterread_refmembers = ""
    prepareforwriting_refmembers = ""
    vecmembers = ""
    # ------------------ create ostream operator --------------------------
    # create colum based output for data members using scientific format
    #
    numColWidth = 12
    ostream_header_string = "id:"
    while len(ostream_header_string) < numColWidth + 1:
      ostream_header_string += " "

    ostream_declaration = ("std::ostream& operator<<( std::ostream& o,const %sCollection& v);\n" % rawclassname)
    ostream_implementation = ("std::ostream& operator<<( std::ostream& o,const %sCollection& v){\n" % rawclassname)
    ostream_implementation += '  std::ios::fmtflags old_flags = o.flags() ; \n'
    ostream_implementation += '  o << "{header_string}" << std::endl ;\n '
    ostream_implementation += '  for(int i = 0; i < v.size(); i++){\n'
    ostream_implementation += '     o << std::scientific << std::showpos '
    ostream_implementation += (' << std::setw(%i) ' % numColWidth)
    ostream_implementation += ' << v[i].id() << " " '
    for m in members:
      name = m["name"]
      t = m["type"]
      colW = numColWidth + 2
      comps = self.reader.components
      compMemStr = ""
      if t in comps:
        nCompMem = 0
        compMemStr += ' ['
        for cm in comps[t]["Members"]:
          if cm != 'ExtraCode':
            nCompMem += 1
            compMemStr += ('%s,' % cm)
        compMemStr += ']'
        colW *= nCompMem
      colName = name[:colW - 2]
      colName += compMemStr
      colName += ":"
      while len(colName) < colW:
        colName += " "
      ostream_header_string += colName

      if(self.get_syntax):
        name = "get" + name[:1].upper() + name[1:]
      if not t.startswith("std::array"):
        ostream_implementation += (' << std::setw(%i) << v[i].%s() << " "' % (numColWidth, name))
    ostream_implementation += '  << std::endl;\n'
    ostream_implementation = ostream_implementation.replace("{header_string}", ostream_header_string)

    # ----------------------------------------------------------------------

    refmembers = definition["OneToOneRelations"]
    refvectors = definition["OneToManyRelations"]
    vectormembers = definition["VectorMembers"]
    nOfRefVectors = len(refvectors)
    nOfRefMembers = len(refmembers)
    if nOfRefVectors + nOfRefMembers > 0:
      # member initialization
      # constructorbody += "\tm_refCollections = new podio::CollRefCollection();\n"
      destructorbody += "\tfor (auto& pointer : m_refCollections) { if (pointer != nullptr) delete pointer; }\n"
      clear_relations += "\tfor (auto& pointer : m_refCollections) { pointer->clear(); }\n"

      for counter, item in enumerate(refvectors):
        name = item["name"]
        klass = item["type"]
        substitutions = {"counter": counter,
                         "class": klass,
                         "name": name}

        mnamespace, klassname, _, __ = demangle_classname(klass)

        # includes
        includes.add(self._build_include(klassname + 'Collection'))

        # FIXME check if it compiles with :: for both... then delete this.
        relations += declarations["relation"].format(namespace=mnamespace, type=klassname, name=name)
        relations += declarations["relation_collection"].format(namespace=mnamespace, type=klassname, name=name)
        initializers += implementations["ctor_list_relation"].format(
            namespace=mnamespace, type=klassname, name=name)

        constructorbody += "\tm_refCollections.push_back(new std::vector<podio::ObjectID>());\n"
        # relation handling in ::create
        create_relations += "\tm_rel_{name}_tmp.push_back(obj->m_{name});\n".format(name=name)
        # relation handling in ::clear
        clear_relations += implementations["clear_relations_vec"].format(name=name)
        clear_relations += implementations["clear_relations"].format(name=name)
        # relation handling in dtor:
        destructorbody += implementations["destroy_relations"].format(name=name)
        # relation handling in push_back
        push_back_relations += "\tm_rel_{name}_tmp.push_back(obj->m_{name});\n".format(name=name)
        # relation handling in ::prepareForWrite
        prepareforwritinghead += "\tint {name}_index =0;\n".format(name=name)
        prepareforwritingbody += self.evaluate_template(
            "CollectionPrepareForWriting.cc.template", substitutions)
        # relation handling in ::settingReferences
        setreferences += self.evaluate_template("CollectionSetReferences.cc.template", substitutions)
        prepareafterread += "\t\tobj->m_%s = m_rel_%s;" % (name, name)

        get_name = name
        if(self.get_syntax):
          get_name = "get" + name[:1].upper() + name[1:]
        ostream_implementation += ('  o << "     %s : " ;\n' % name)
        ostream_implementation += ('  for(unsigned j=0,N=v[i].%s_size(); j<N ; ++j)\n' % name)
        ostream_implementation += ('    o << v[i].%s(j).id() << " " ; \n' % get_name)
        ostream_implementation += '  o << std::endl ;\n'

      for counter, item in enumerate(refmembers):
        name = item["name"]
        klass = item["type"]
        mnamespace = ""
        klassname = klass
        if "::" in klass:
          mnamespace, klassname = klass.split("::")
        substitutions = {"counter": counter + nOfRefVectors,
                         "class": klass,
                         "rawclass": klassname,
                         "name": name}

        # includes
        includes.add(self._build_include(klassname + 'Collection'))
        # constructor call
        initializers += implementations["ctor_list_relation"].format(
            namespace=mnamespace, type=klassname, name=name)
        # member
        relations += declarations["relation"].format(namespace=mnamespace, type=klassname, name=name)
        constructorbody += "\tm_refCollections.push_back(new std::vector<podio::ObjectID>());\n"
        # relation handling in ::clear
        clear_relations += implementations["clear_relations"].format(name=name)
        # relation handling in dtor:
        destructorbody += implementations["destroy_relations"].format(name=name)
        # relation handling in ::prepareForWrite
        prepareforwriting_refmembers += implementations["prep_writing_relations"].format(
            name=name, i=(counter + nOfRefVectors))
        # relation handling in ::settingReferences
        prepareafterread_refmembers += self.evaluate_template(
            "CollectionSetSingleReference.cc.template", substitutions)

        get_name = name
        if(self.get_syntax):
          get_name = "get" + name[:1].upper() + name[1:]
        ostream_implementation += ('  o << "     %s : " ;\n' % name)
        ostream_implementation += ('  o << v[i].%s().id() << std::endl;\n' % get_name)

    if len(vectormembers) > 0:
      includes.add('#include <numeric>')
    for counter, item in enumerate(vectormembers):
      name = item["name"]
      klass = item["type"]
      get_name = name
      if(self.get_syntax):
        get_name = "get" + name[:1].upper() + name[1:]

      vecmembers += declarations["vecmembers"].format(type=klass, name=name)
      constructorbody += "\tm_vecmem_info.push_back( std::make_pair( \"{type}\", &m_vec_{name} )) ; \n".format(
          type=klass, name=name)
      constructorbody += "\tm_vec_{name} = new std::vector<{type}>() ;\n".format(type=klass, name=name)
      create_relations += "\tm_vecs_{name}.push_back(obj->m_{name});\n".format(name=name)
      destructorbody += "\tif(m_vec_{name} != nullptr) delete m_vec_{name};\n".format(name=name)
      clear_relations += "\tm_vec_{name}->clear();\n".format(name=name)
      clear_relations += "\tm_vecs_{name}.clear();\n".format(name=name)
      prepareforwritinghead += "\tint {name}_size = ".format(name=name)
      prepareforwritinghead += "std::accumulate( m_entries.begin(), m_entries.end(), 0, "
      prepareforwritinghead += "[](int sum, const {rawclassname}Obj*  obj)".format(rawclassname=rawclassname)
      prepareforwritinghead += "{{ return sum + obj->m_{name}->size();}} );\n".format(name=name)
      prepareforwritinghead += "\tm_vec_{name}->reserve( {name}_size );\n".format(name=name)
      prepareforwritinghead += "\tint {name}_index =0;\n".format(name=name)

      prepareforwritingbody += self.evaluate_template(
          "CollectionPrepareForWritingVecMember.cc.template", {"name": name})
      push_back_relations += "\tm_vecs_{name}.push_back(obj->m_{name});\n".format(name=name)

      prepareafterread += "\t\tobj->m_{name} = m_vec_{name};\n".format(name=name)

      ostream_implementation += ('  o << "     %s : " ;\n' % name)
      ostream_implementation += ('  for(unsigned j=0,N=v[i].%s_size(); j<N ; ++j)\n' % name)
      ostream_implementation += ('    o << v[i].%s(j) << " " ; \n' % get_name)
      ostream_implementation += '  o << std::endl ;\n'

    ostream_implementation += '  }\no.flags(old_flags);\n'
    ostream_implementation += "  return o ;\n}\n"

    substitutions = {"name": rawclassname,
                     "classname": classname,
                     "constructorbody": constructorbody,
                     "destructorbody": destructorbody,
                     "prepareforwritinghead": prepareforwritinghead,
                     "prepareforwritingbody": prepareforwritingbody,
                     "prepareforwriting_refmembers": prepareforwriting_refmembers,
                     "setreferences": setreferences,
                     "prepareafterread": prepareafterread,
                     "prepareafterread_refmembers": prepareafterread_refmembers,
                     "includes": self._join_set(includes),
                     "initializers": initializers,
                     "relations": relations,
                     "create_relations": create_relations,
                     "clear_relations": clear_relations,
                     "push_back_relations": push_back_relations,
                     "vecmembers": vecmembers,
                     "package_name": self.package_name,
                     "vectorized_access_declaration": vectorized_access_decl,
                     "vectorized_access_implementation": vectorized_access_impl,
                     "namespace_open": namespace_open,
                     "namespace_close": namespace_close,
                     "ostream_declaration": ostream_declaration,
                     "ostream_implementation": ostream_implementation
                     }
    self.fill_templates("Collection", substitutions)
    self.created_classes.append("%sCollection" % classname)

  def create_PrintInfo(self, classname, definition):
    namespace, rawclassname, namespace_open, namespace_close = demangle_classname(classname)

    toFormattingStrings = {"char": "3", "unsigned char": "3",
                           "long": "11", "longlong": "22", "bool": "1", "int": ""}
    formats = {"char": 3, "unsigned char": 3, "long": 11, "longlong": 22, "bool": 1, }

    outputSingleObject = 'o << "' + classname + ':" << std::endl; \n     o << '
    WidthIntegers = ""
    widthOthers = ""
    findMaximum = ""
    setFormats = ""
    formattedOutput = ""
    tableHeader = ""
    for member in definition["Members"]:
      name = member["name"]
      lengthName = len(name)
      klass = member["type"]
      if(klass in toFormattingStrings and not klass == "int"):
        if(formats[klass] > lengthName):
          widthOthers = widthOthers + "  int" + " " + name + "Width = " + toFormattingStrings[klass] + " ; \n"
        else:
          widthOthers = widthOthers + "  int" + " " + name + "Width = " + str(lengthName) + " ; \n"
      elif klass == "int":
        findMaximum += "\n    int " + name + "Max ; \n"
        findMaximum += "    " + name + "Width = 1 ; \n "
        findMaximum += "     for(int i = 0 ; i < value.size(); i++){ \n"
        findMaximum += "         if( value[i].get" + name[:1].upper() + name[1:] + "() > 0 ){ \n"
        findMaximum += "            if(" + name + \
            "Max <  value[i].get" + name[:1].upper() + name[1:] + "()){ \n"
        findMaximum += "               " + name + "Max = value[i].get" + name[:1].upper() + name[1:] + "();"
        findMaximum += "\n            } \n"
        findMaximum += "\n         } \n"
        findMaximum += "         else if( -" + name + \
            "Max / 10 > value[i].get" + name[:1].upper() + name[1:] + "()){ \n"
        findMaximum += "             " + name + \
            "Max = - value[i].get" + name[:1].upper() + name[1:] + "() * 10; "
        findMaximum += "\n         } \n"
        findMaximum += "     } \n"
        setFormats += "\n    while(" + name + "Max != 0){ \n"
        setFormats += "       " + name + "Width++; \n       " + name + "Max = " + name + "Max / 10; \n    } \n"
        setFormats += "   if(" + name + "Width < " + str(lengthName) + \
            "){ " + name + "Width = " + str(lengthName) + ";} \n"
        WidthIntegers = WidthIntegers + "  " + klass + " " + name + "Width = 1 ; \n"
      elif(klass == "double" or klass == "float"):
        if(lengthName > 12):
          widthOthers = widthOthers + "  int" + " " + name + "Width = " + str(lengthName) + " ; \n"
        else:
          widthOthers = widthOthers + "  int" + " " + name + "Width = 12 ; \n"
      elif(klass == "DoubleThree" or klass == "FloatThree"):
        if(lengthName > 38):
          widthOthers = widthOthers + "  int" + " " + name + "Width = " + str(lengthName) + " ; \n"
        else:
          widthOthers += "  int" + " " + name + "Width = 38 ; \n"
      elif(klass == "IntTwo"):
        if(lengthName > 24):
          widthOthers = widthOthers + "  int" + " " + name + "Width = " + str(lengthName) + " ; \n"
        else:
          widthOthers += "  int" + " " + name + "Width = 24 ; \n"
      else:
        widthOthers = widthOthers + "  int" + " " + name + "Width = 38 ; \n"
      if(klass != "StingVec"):
        tableHeader += 'std::setw(' + classname + "PrintInfo::instance()." + \
            name + 'Width) << "' + name + '" << "|" << '
        if(klass == "DoubleThree" or klass == "FloatThree" or klass == "StringVec"):
          formattedOutput += " value[i].get" + name[:1].upper() + name[1:] + '() << " "' + " << "
        else:
          formattedOutput += " std::setw(" + classname + "PrintInfo::instance()." + name + "Width)"
          formattedOutput += " << value[i].get" + name[:1].upper() + name[1:] + '() << " "' + " << "
          outputSingleObject += '"' + klass + '" << " " << "' + name + '" << " "' + \
              " << value.get" + name[:1].upper() + name[1:] + '() << " "' + " << "

    substitutions = {"name": rawclassname,
                     "widthIntegers": WidthIntegers,
                     "widthOthers": widthOthers,
                     "findMaximum": findMaximum,
                     "setFormats": setFormats,
                     "formattedOutput": formattedOutput,
                     "tableHeader": tableHeader,
                     "outputSingleObject": outputSingleObject
                     }

    # TODO: add loading of code from external files

    self.fill_templates("PrintInfo", substitutions)
    self.created_classes.append(classname)

  def create_component(self, classname, components):
    """ Create a component class to be used within the data types
        Components can only contain simple data types and no user
        defined ones
    """
    includes = set()
    members = []

    for name, klass in components.items():
      if name != "ExtraCode":
        if "::" in klass:
          namespace, klassname = klass.split("::") # TODO: is this all covered by validation?
          scoped_type = '::{namespace}::{type}'.format(namespace=namespace, type=klassname)
          members.append([scoped_type, name])
        else:
          klassname = klass
          members.append([klass, name])

        # Handle includes
        if klass in self.reader.components:
          includes.add(self._build_include(klassname))

        # TODO: Move to own function?
        array_match = ClassDefinitionValidator.array_re.search(klass)
        if array_match:
          includes.add('#include <array>')
          array_type = array_match.group(1)
          if array_type not in self.buildin_types:
            if "::" in array_type:
              array_type = array_type.split("::")[1]
            includes.add(self._build_include(array_type))


      # handle user provided extra code
      if 'ExtraCode' in components:
        extracode = components['ExtraCode']
        extracode_declarations = extracode.get("declaration", "")
        includes.update(set(extracode.get("includes", "").split('\n')))
      else:
        extracode_declarations = ""

      members_decl = '\n'.join(('  {t} {n};'.format(t=typ, n=name) for (typ, name) in members))

    _, rawclassname, namespace_open, namespace_close = demangle_classname(classname)
    substitutions = {"ostreamComponents": ostream_component(members, classname),
                     "includes": self._join_set(includes),
                     "members": members_decl,
                     "extracode_declarations": extracode_declarations,
                     "name": rawclassname,
                     "package_name": self.package_name,
                     "namespace_open": namespace_open,
                     "namespace_close": namespace_close
                     }
    self.fill_templates("Component", substitutions)
    self.component_members[classname] = members
    self.created_classes.append(classname)


  def create_obj(self, classname, definition):
    """ Create an obj class containing all information
        relevant for a given object.
    """
    namespace, rawclassname, namespace_open, namespace_close = demangle_classname(classname)

    relations = ""
    includes = set()
    includes_cc = set()
    forward_declarations = ""
    forward_declarations_namespace = {"": []}
    initialize_relations = ""
    set_relations = ""
    deepcopy_relations = ""
    delete_relations = ""
    delete_singlerelations = ""
    refvectors = definition["OneToManyRelations"]
    singleRelations = definition["OneToOneRelations"]
    # do includes and forward declarations for
    # oneToOneRelations and do proper cleanups
    for item in singleRelations:
      name = item["name"]
      klass = item["type"]
      klassname = klass
      mnamespace = ""
      if klass not in self.buildin_types:
        if "::" in klass:
          mnamespace, klassname = klass.split("::")
          klassWithQualifier = "::" + mnamespace + "::Const" + klassname
          if mnamespace not in forward_declarations_namespace:
            forward_declarations_namespace[mnamespace] = []
        else:
          klassWithQualifier = "Const" + klass
      else:
        klassWithQualifier = klass

      if mnamespace != "":
        relations += "  ::%s::Const%s* m_%s;\n" % (mnamespace, klassname, name)
      else:
        relations += "  Const%s* m_%s;\n" % (klassname, name)

      if klass not in self.buildin_types:
        if klass != classname:
          forward_declarations_namespace[mnamespace] += ['class Const%s;\n' % (klassname)]
          includes_cc.add(self._build_include("%sConst" % klassname))
          initialize_relations += ", m_%s(nullptr)\n" % (name)
          # for deep copy initialise as nullptr and set in copy ctor body
          # if copied object has non-trivial relation
          deepcopy_relations += ", m_%s(nullptr)" % (name)
          set_relations += implementations["set_relations"].format(name=name, klass=klassWithQualifier)
        delete_singlerelations += "\t\tif (m_%s != nullptr) delete m_%s;\n" % (name, name)

    for nsp in forward_declarations_namespace:
      if nsp != "":
        forward_declarations += "namespace %s {" % nsp
      forward_declarations += "".join(forward_declarations_namespace[nsp])
      if nsp != "":
        forward_declarations += "}\n"

    if len(refvectors + definition["VectorMembers"]) != 0:
      includes.add("#include <vector>")

    for item in refvectors + definition["VectorMembers"]:
      name = item["name"]
      klass = item["type"]
      if klass not in self.buildin_types:
        if klass not in self.reader.components:
          if "::" in klass:
            mnamespace, klassname = klass.split("::")
            klassWithQualifier = "::" + mnamespace + "::Const" + klassname
          else:
            klassWithQualifier = "Const" + klass
        else:
          klassWithQualifier = klass
        relations += "\tstd::vector<%s>* m_%s;\n" % (klassWithQualifier, name)
        initialize_relations += ", m_%s(new std::vector<%s>())" % (name, klassWithQualifier)
        deepcopy_relations += ", m_%s(new std::vector<%s>(*(other.m_%s)))" % (name, klassWithQualifier, name)
        if klass == classname:
          includes_cc.add(self._build_include(rawclassname))
        else:
          if "::" in klass:
            mnamespace, klassname = klass.split("::")
            includes.add(self._build_include(klassname))
          else:
            includes.add(self._build_include(klass))
      else:
        relations += "\tstd::vector<%s>* m_%s;\n" % (klass, name)
        initialize_relations += ", m_%s(new std::vector<%s>())" % (name, klass)
        deepcopy_relations += ", m_%s(new std::vector<%s>(*(other.m_%s)))" % (name, klass, name)

      delete_relations += "\t\tdelete m_%s;\n" % (name)
    substitutions = {"name": rawclassname,
                     "includes": self._join_set(includes),
                     "includes_cc": self._join_set(includes_cc),
                     "forward_declarations": forward_declarations,
                     "relations": relations,
                     "initialize_relations": initialize_relations,
                     "deepcopy_relations": deepcopy_relations,
                     "delete_relations": delete_relations,
                     "delete_singlerelations": delete_singlerelations,
                     "namespace_open": namespace_open,
                     "namespace_close": namespace_close,
                     "set_deepcopy_relations": set_relations
                     }
    self.fill_templates("Obj", substitutions)
    self.created_classes.append(classname + "Obj")

  def prepare_vectorized_access(self, classname, members):
    implementation = ""
    declaration = ""
    for member in members:
      name = member["name"]
      klass = member["type"]
      substitutions = {"classname": classname,
                       "member": name,
                       "type": klass
                       }
      if klass not in self.buildin_types:
        substitutions["type"] = "class %s" % klass
      implementation += self.evaluate_template("CollectionReturnArray.cc.template", substitutions)
      declaration += "\ttemplate<size_t arraysize>\n\t" \
          "const std::array<%s,arraysize> %s() const;\n" % (klass, name)
    return declaration, implementation

  def write_file(self, name, content):
    # dispatch headers to header dir, the rest to /src
    # fullname = os.path.join(self.install_dir,self.package_name,name)
    if name.endswith("h"):
      fullname = os.path.join(self.install_dir, self.package_name, name)
    else:
      fullname = os.path.join(self.install_dir, "src", name)
    if not self.dryrun:
      if self.clang_format:
        cfproc = subprocess.Popen(self.clang_format, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        content = cfproc.communicate(input=content.encode())[0].decode()

      try:
        with open(fullname, 'r') as f:
          existing_content = f.read()
          changed = existing_content != content
      except FileNotFoundError:
        changed = True

      if changed:
        with open(fullname, 'w') as f:
          f.write(content)


  def evaluate_template(self, filename, substitutions):
    """ reads in a given template, evaluates it
        and returns result
    """
    templatefile = os.path.join(self.template_dir, filename)
    with open(templatefile, 'r') as tempfile:
      template = tempfile.read()
    return string.Template(template).substitute(substitutions)

  def fill_templates(self, category, substitutions):
    # add common include subfolder if required
    substitutions['incfolder'] = '' if not self.include_subfolder else self.package_name + '/'
    substitutions['PACKAGE_NAME'] = self.package_name.upper()

    # depending on which category is passed different naming conventions apply
    # for the generated files. Additionally not all categories need source files.
    # Listing the special cases here
    fn_base = {
      'Data': 'Data',
      'Obj': 'Obj',
      'ConstObject': 'Const',
      'PrintInfo': 'PrintInfo',
      'Object': '',
      'Component': ''
    }.get(category, category)

    endings = {
      'Data': ('h',),
      'Component': ('h',),
      'PrintInfo': ('h',)
    }.get(category, ('h', 'cc'))

    for ending in endings:
      templatefile = "%s.%s.template" % (category, ending)
      content = self.evaluate_template(templatefile, substitutions)
      filename = "%s%s.%s" % (substitutions["name"], fn_base, ending)
      self.write_file(filename, content)

  def _build_include(self, classname):
    """Return the include statement."""
    if self.include_subfolder:
      classname = os.path.join(self.package_name, classname)
    return '#include "%s.h"' % classname

  @staticmethod
  def _join_set(aSet):
    """Return newline-joined sorted set."""
    return '\n'.join(sorted([inc.strip() for inc in aSet]))


##########################
if __name__ == "__main__":
  import argparse
  parser = argparse.ArgumentParser(description='Given a description yaml file this script generates '
                                   'the necessary c++ files in the target directory')

  parser.add_argument('description', help='yaml file describing the datamodel')
  parser.add_argument('targetdir', help='Target directory where the generated data classes will be put. '
                      'Header files will be put under <targetdir>/<packagename>/*.h. '
                      'Source files will be put under <targetdir>/src/*.cc')
  parser.add_argument('packagename', help='Name of the package.')

  parser.add_argument('-q', '--quiet', dest='verbose', action='store_false', default=True,
                      help='Don\'t write a report to screen')
  parser.add_argument('-d', '--dryrun', action='store_true', default=False,
                      help='Do not actually write datamodel files')
  parser.add_argument('-c', '--clangformat', action='store_true', default=False,
                      help='Apply clang-format when generating code (with -style=file)')

  args = parser.parse_args()

  install_path = args.targetdir
  project = args.packagename

  for sub_dir in ('src', project):
    directory = os.path.join(install_path, sub_dir)
    if not os.path.exists(directory):
      os.makedirs(directory)

  gen = ClassGenerator(args.description, args.targetdir, args.packagename,
                       verbose=args.verbose, dryrun=args.dryrun)
  if args.clangformat:
    gen.clang_format = get_clang_format()
  gen.process()
