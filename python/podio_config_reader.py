import yaml
import copy

class ClassDefinitionValidator(object):

  valid_keys = (
    "Description",
    "Author",
    "Members",
    "VectorMembers",
    "OneToOneRelations",
    "OneToManyRelations",
    "TransientMembers",
    "Typedefs",
    "ExtraCode"
  )

  buildin_types = [ "int", "long", "float", "double", "unsigned int", "unsigned", "short", "bool", "longlong", "ulonglong"]

  def __init__(self):
    self.components = {}
    self.datatypes = {}

  @staticmethod
  def check_keys(name, definition):
    """Check that only valid keys are provided"""
    for key in definition:
      if key not in ClassDefinitionValidator.valid_keys:
        raise Exception("%s defines invalid category '%s' " %(name,key))

  @staticmethod
  def check_datatype(name, definition):
    ClassDefinitionValidator.check_keys(name, definition)

  def check_component(self, name, definition):
    """Check that components only contain simple types or other components"""
    for klass in definition.itervalues():
      if not (klass in self.buildin_types or klass in self.components.keys()):
        raise Exception("'%s' defines a member of a type '%s' which is not allowed in a component!" %(name, klass))

  def check_components(self,components):
    self.components = components
    for klassname, value in components.iteritems():
      self.check_component(klassname, value)

class PodioConfigReader(object):

  def __init__(self,yamlfile):
    self.yamlfile = yamlfile
    self.datatypes = {}
    self.components = {}

  @staticmethod
  def check_class(klass):
    # check whether it is an array
    if "[" in klass:
      theType = klass.split("[")[0]
      number  = klass.split("[")[1].split("]")[0]
      # transform it into std::array
      #klass = "std::array<%s,%s>" %(theType, number);
    return klass

  @staticmethod
  def handle_extracode(definition):
    return copy.deepcopy(definition)

  def read(self):
    validator = ClassDefinitionValidator()
    stream = open(self.yamlfile, "r")
    content = yaml.load(stream)

    if content.has_key("datatypes"):
      for klassname, value in content["datatypes"].iteritems():
        validator.check_datatype(klassname, value);
        datatype = {}
        datatype["Description"] = value["Description"]
        datatype["Author"] = value["Author"]
        for category in ("Members","VectorMembers","OneToOneRelations","OneToManyRelations","TransientMembers","Typedefs"):
          definitions = []
          if value.has_key(category):
            for definition in value[category]:
              klass, name = definition.split()[:2]
              klass = self.check_class(klass)
              comment = definition.split("//")[1]
              definitions.append({"name": name, "type": klass, "description" : comment})
            datatype[category] = definitions
          else:
            datatype[category] = []
        if value.has_key("ExtraCode"):
           datatype["ExtraCode"] = self.handle_extracode(value["ExtraCode"])
        self.datatypes[klassname] = datatype
    if "components" in content.keys():
      validator.check_components(content["components"])
      for klassname, value in content["components"].iteritems():
        component = {"Members": value}
        self.components[klassname] = component



