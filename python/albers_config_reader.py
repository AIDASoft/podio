import yaml

class AlbersConfigReader(object):

  def __init__(self,yamlfile):
    self.yamlfile = yamlfile

  @staticmethod
  def check_class(klass):
    # check whether it is an array
    if "[" in klass:
      theType = klass.split("[")[0]
      number  = klass.split("[")[1].split("]")[0]
      # transform it into std::array
      klass = "std::array<%s,%s>" %(theType, number);
    return klass

  def read(self):
    stream = open(self.yamlfile, "r")
    content = yaml.load(stream)
    datatypes = {}
    if content.has_key("datatypes"):
      for klassname, value in content["datatypes"].iteritems():
        datatype = {}
        datatype["description"] = value["description"]
        datatype["author"] = value["author"]
        for category in ("members","VectorMembers","OneToOneRelations","OneToManyRelations"):
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
        datatypes[klassname] = datatype
    return datatypes
