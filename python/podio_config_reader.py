import yaml
import re

class PodioConfigReader(object):

  def __init__(self,yamlfile):
    self.yamlfile = yamlfile
    self.pattern = re.compile("[a-zA-Z0-9]+[\s]+[a-zA-Z0-9]+[\s]+[//\s]+")

  @staticmethod
  def check_class(klass):
    # check whether it is an array
    if "[" in klass:
      theType = klass.split("[")[0]
      number  = klass.split("[")[1].split("]")[0]
      # transform it into std::array
      #klass = "std::array<%s,%s>" %(theType, number);
    return klass

  def read(self):
    stream = open(self.yamlfile, "r")
    content = yaml.load(stream)
    datatypes = {}
    if content.has_key("datatypes"):
      for klassname, value in content["datatypes"].iteritems():
        datatype = {}
        datatype["Description"] = value["Description"]
        datatype["Author"] = value["Author"]
        for category in ("Members","VectorMembers","OneToOneRelations","OneToManyRelations","TransientMembers"):
          definitions = []
          if value.has_key(category):
            for definition in value[category]:
              if self.pattern.match(definition):
                klass, name = definition.split()[:2]
                klass = self.check_class(klass)
                comment = definition.split("//")[1]
                definitions.append({"name": name, "type": klass, "description" : comment})
              else:
                raise Exception("'%s'\n Definitions in datatypes should have a following form:\n <type> <name> // <comment>\n eg. int Number // event number" %(definition))
            datatype[category] = definitions
          else:
            datatype[category] = []
        datatypes[klassname] = datatype
    return datatypes
