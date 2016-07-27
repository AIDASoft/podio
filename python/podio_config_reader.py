import yaml
import copy


def parseMember(string):
    """
    extract klass, name, and comment from
     - int name // comment
    """
    klass, name = string.split()[:2]
    comment = string.split("//")[1]
    return {"name": name,
            "type": klass,
            "description": comment
            }


class ClassDefinitionValidator(object):
    """
    Validate the input yaml file for the
    most obvious problems.
    """

    valid_keys = (
      "Description",
      "Author",
      "Members",
      "VectorMembers",
      "OneToOneRelations",
      "OneToManyRelations",
      "TransientMembers",
      "Typedefs",
      "ExtraCode",
      "ConstExtraCode"
    )

    buildin_types = ["int", "long", "float", "double",
                     "unsigned int", "unsigned",
                     "short", "bool", "longlong",
                     "ulonglong", "std::string"]

    def __init__(self, configuration):
        self.components = {}
        self.datatypes = {}
        if "datatypes" in configuration:
            self.datatypes = configuration["datatypes"]
        if "components" in configuration:
            self.components = configuration["components"]

    @staticmethod
    def check_keys(name, definition):
        """Check that only valid keys are provided"""
        for key in definition:
            if key not in ClassDefinitionValidator.valid_keys:
                raise Exception("%s defines invalid category '%s' "
                                % (name, key))

    def check_datatype(self, name, definition):
        self.check_keys(name, definition)
        for category in ("Author", "Description"):
            if category not in definition.keys():
                raise Exception("%s does not define '%s'." % (name, category))
        for category in ("Members", "OneToOneRelations", "OneToManyRelations"):
            if category in definition:
                self.check_members(name, definition[category])
        # TODO: handling of vector members

    def check_members(self, name, members):
        for item in members:
            member = parseMember(item)
            theType = member["type"]
            return  # TODO
            if theType not in self.buildin_types and \
               theType not in self.datatypes.keys() and \
               theType not in self.components.keys():
                raise Exception("%s defines a member of not allowed type %s"
                                % (name, theType))

    def check_component(self, name, definition):
        """
        Check that components only contain simple types
        or other components
        """
        for mem in definition.keys():
            klass = definition[mem]
            if not (mem == "ExtraCode" or klass in self.buildin_types \
                    or klass in self.components.keys()):
                raise Exception("'%s' defines a member of a type '%s'"
                                % (name, klass) +
                                "which is not allowed in a component!")

    def check_components(self, components):
        for klassname, value in components.iteritems():
            self.check_component(klassname, value)


class PodioConfigReader(object):

    def __init__(self, yamlfile):
        self.yamlfile = yamlfile
        self.datatypes = {}
        self.components = {}
        self.options = {
            # should getters / setters be prefixed with get / set?
            "getSyntax": False,
            # should POD members be exposed with getters/setters in classes that have them as members?
            "exposePODMembers": True}

    @staticmethod
    def handle_extracode(definition):
        return copy.deepcopy(definition)

    def read(self):
        stream = open(self.yamlfile, "r")
        content = yaml.load(stream)
        validator = ClassDefinitionValidator(content)
        if "datatypes" in content:
            for klassname, value in content["datatypes"].iteritems():
                validator.check_datatype(klassname, value)
                datatype = {}
                datatype["Description"] = value["Description"]
                datatype["Author"] = value["Author"]
                for category in ("Members",
                                 "VectorMembers",
                                 "OneToOneRelations",
                                 "OneToManyRelations",
                                 "TransientMembers",
                                 "Typedefs"):
                    definitions = []
                    if category in value:
                        for definition in value[category]:
                            definitions.append(parseMember(definition))
                        datatype[category] = definitions
                    else:
                        datatype[category] = []
                if "ExtraCode" in value:
                    datatype["ExtraCode"] = self.handle_extracode(
                                                value["ExtraCode"])
                if "ConstExtraCode" in value:
                    datatype["ConstExtraCode"] = self.handle_extracode(
                                                     value["ConstExtraCode"])
                self.datatypes[klassname] = datatype
        if "components" in content.keys():
            validator.check_components(content["components"])
            for klassname, value in content["components"].iteritems():
                component = {"Members": value}
                self.components[klassname] = component
        if "options" in content.keys():
            for option, value in content["options"].iteritems():
                self.options[option] = value
