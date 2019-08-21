#!/usr/bin/env python
import os
import string
import pickle
import subprocess
import re
from collections import OrderedDict
import copy
from pprint import pprint
from sys import exit
from podio_config_reader import PodioConfigReader, ClassDefinitionValidator
from podio_templates import declarations, implementations
thisdir = os.path.dirname(os.path.abspath(__file__))

class ClassGenerator(object):

	def __init__(self, yamlfile, install_dir, package_name, verbose=True, dryrun=False):

		#print 'ClassGenerator __init__ TRIGGERED\n'
		self.yamlfile = yamlfile
		self.install_dir = install_dir
		self.package_name = package_name
		self.template_dir = os.path.join(thisdir, "../templates")
		self.verbose = verbose
		self.reader = PodioConfigReader(yamlfile)
		self.warnings = []
		self.dryrun = dryrun
		self.namespace = {}

		# c++ to hdf5 datatype map;
		# note: bool type not found in hdf5 pre-defined datatypes.
		self.dtype_map = {'int': 'PredType::NATIVE_INT',     \
				'long': 'PredType::NATIVE_LONG',    \
				'float': 'PredType::NATIVE_FLOAT', \
				'double': 'PredType::NATIVE_DOUBLE',\
				'unsigned int': 'PredType::NATIVE_UINT',\
				'unsigned long': 'PredType::NATIVE_ULONG',\
				'short': 'PredType::NATIVE_SHORT',\
				'long long': 'PredType::NATIVE_LLONG',\
				'unsigned long long': 'PredType::NATIVE_ULLONG',\
				'signed char': 'PredType::NATIVE_SCHAR', \
				'unsigned char': 'PredType::NATIVE_UCHAR',\
				'char': 'PredType::NATIVE_CHAR',    \
				'long double' : 'PredType::NATIVE_LDOUBLE'
				}
		self.predef = self.dtype_map.keys()

		# different map for array type, Predefined native datatypes
		self.a_type_map = {'int': 'H5T_NATIVE_INT',     \
				'long': 'H5T_NATIVE_LONG',    \
				'float': 'H5T_NATIVE_FLOAT', \
				'double': 'H5T_NATIVE_DOUBLE',\
				'unsigned int': 'H5T_NATIVE_UINT',\
				'unsigned long': 'H5T_NATIVE_ULONG',\
				'short': 'H5T_NATIVE_SHORT',\
				'long long': 'H5T_NATIVE_LLONG',
				'unsigned long long': 'H5T_NATIVE_ULLONG',\
				'signed char': 'H5T_NATIVE_SCHAR', \
				'unsigned char': 'H5T_NATIVE_UCHAR',\
				'char': 'H5T_NATIVE_CHAR',    \
				'long double' : 'H5T_NATIVE_LDOUBLE'\
				}

	def configure_clang_format(self, apply):
		if not apply:
		    self.clang_format = []
		    return
		try:
		    cformat_exe = subprocess.check_output(['which', 'clang-format']).strip()
		except subprocess.CalledProcessError:
		    print ("ERROR: Cannot find clang-format executable")
		    print ("       Please make sure it is in the PATH.")
		    self.clang_format = []
		    return
		self.clang_format = [cformat_exe, "-i",  "-style=file", "-fallback-style=llvm"]

	def process(self):
		self.reader.read()
		self.getSyntax = self.reader.options["getSyntax"]
		self.expose_pod_members = self.reader.options["exposePODMembers"]
		#pprint(self.reader.components)
		#pprint(self.reader.datatypes)


		# First we want the components and datatypes to have the same
		# structure
		mod_dtype = self.process_datatypes(self.reader.datatypes)

		# next we want to combine the components and datatypes dictonaries
		content = copy.deepcopy(mod_dtype)
		content.update(self.reader.components)

		# clean the contents by getting rid of ExtraCode in components
		# and also building a list of namespace
		clean_content = self.clean(content)

		# we only need to iterate over the datatypes
		# but we need to clean_content to remember the components

		for name in mod_dtype:
			if clean_content[self.strip_namespace(name)]['Members']:
				self.write_hdf5_header(self.strip_namespace(name))
				self.write_hdf5_src(self.strip_namespace(name), clean_content)

	def clean(self, content):

		# first get rid of extraCode if any
		for name, components in content.items():
			components['Members'].pop("ExtraCode", None)

		# next gather the required namespace for each datatype
		for name in content:
			order = self.topological_sort(name, content)
			new_name = name
			if "::" in name:
				new_name = name[name.rindex("::")+2:]

			self.namespace[new_name] = set([])
			for type in order:
				if "::" in type:
					namespace = set(type.split("::")[:-1])
					#print(namespace)
					self.namespace[new_name] = self.namespace[new_name].union(namespace)

		#pprint(self.namespace)
		#exit(1)

		# now get rid of the namespace in each word
		new_content = self.remove_namespace(content)

		return new_content

	def process_datatypes(self, content):
		mod_dtype = {}
		for datatype, dic  in content.items():
			# extract Members
			members = dic['Members']
			temp = {}
			for d in members:
			    key = d['name']
			    value = d['type']
			    temp[key] = value

			mod_dtype[datatype] = {'Members':temp}

			# extract oneToManyRelations
			one_to_many = dic['OneToManyRelations']
			temp = {}
			for d in one_to_many:
			    k1 = d['name'] + '_begin'
			    k2 = d['name'] + '_end'
			    value = 'unsigned int'
			    temp[k1] = value
			    temp[k2] = value

			mod_dtype[datatype]['Members'].update(temp)

			# extract Vector Members
			vec_member = dic['VectorMembers']
			temp = {}
			for d in vec_member:
			    k1 = d['name'] + '_begin'
			    k2 = d['name'] + '_end'
			    value = 'unsigned int'
			    temp[k1] = value
			    temp[k2] = value

			mod_dtype[datatype]['Members'].update(temp)

		return mod_dtype

	def write_hdf5_header(self, name):

		# declare header guards
		header_guards = "#ifndef H5" + name.upper() + "DATA_H\n" + \
						"#define H5" + name.upper() + "DATA_H\n\n\n"

		# declare includes
		includes = self.get_includes(name)

		# declare the namespaces being used
		namespace = self.get_namespace(name)

		# declare dataset variable and rank
		# first convert EventInfo to EVENT_INFO_DATA
		const_dec = 'const H5std_string {}DATA("{}Data");\n'.format(name.upper(),name) + \
					'const int RANK_{} = 1;\n\n\n'.format(name.upper())

		before_class_dec = header_guards + includes + namespace + const_dec

		# Begin Class declaration

		after_class_dec = "class H5{}Collection : public podio::H5Collections\n".format(name)+ \
					"{\n" + \
					"\tpublic:\n" + \
					"\t\tCompType h5_datatype;\n" + \
					"\t\tunsigned int event_count;\n" + \
					"\n\n"+\
					"\tpublic:\n"+\
					"\t\tvoid writeCollection(CollectionBase* c, H5File& file);\n"+\
					"\t\tstatic H5{}Collection* getInstance();\n".format(name)+\
					"\t\t{}Collection* get_collection(const std::string& name, EventStore* m_store);\n".format(name) + \
					"\n\n" +\
					"\tprivate:\n" +\
					"\t\tstatic H5{}Collection* instance;\n".format(name) +\
					"\t\tH5{}Collection();\n".format(name) +\
					"};\n\n" +\
					"/* Initially the instance is NULL */\n"+\
					"H5{}Collection* H5{}Collection::instance = 0;\n\n\n".format(name, name) +\
					"#endif"

		content = before_class_dec + after_class_dec
		filename = "H5{}Collection".format(name) + ".h"
		self.write_file(filename, content)

	def get_neighbours(self, varName, content):
	    data_dic = content[varName]['Members']
	    n_list = []
	    for varName, d_name in data_dic.items():
			if 'array' not in d_name and 'string' not in d_name and d_name not in self.dtype_map:
				n_list.append(d_name)
			if 'array' in d_name:
				st_index = d_name.find('<') + 1
				end_index = d_name.find(',')
				data_type = d_name[st_index:end_index].strip()
				if data_type not in self.a_type_map:
					# then it is a compound type
					n_list.append(data_type)

	    return n_list

	def topological_sort(self, varName, content):
	    result = []
	    seen = set()

	    def recursive_helper(varName):
	        n_list = self.get_neighbours(varName, content)
	        for neighbor in n_list:
	            if neighbor not in seen:
	                seen.add(neighbor)
	                recursive_helper(neighbor)
	        result.insert(0, varName)

	    recursive_helper(varName)

	    # this returns the order in which one should define the compound types
	    return result[::-1]

	def get_constructor(self, name, content):
		constructor_dec = "H5{}Collection::H5{}Collection()\n".format(name, name) + "{\n"

		order = self.topological_sort(name, content)

		struct_array_dec = ''
		string_dec = ''
		comp_dec = ''
		for struct_name in order:

			# declare dimension of arrays if any for compound type
			for varName, dtype in content[struct_name]['Members'].items():
				if 'array' in dtype:
					d = re.findall(r'\d+', dtype)[0]
					struct_array_dec += '\thsize_t %s_array_dim[] = {%s};\n' % (varName, d)
				if 'string' in dtype:
					string_dec += '\tStrType vlst_{}(0, H5T_VARIABLE);\n'.format(dtype)

			# declare compound type
			if(struct_name == name):
				comp_dec += "\n\th5_datatype = CompType(sizeof({}Data));\n".format(name)
			else:
				comp_dec += '\n\tCompType mtype_{}(sizeof({}));\n'.format(struct_name, struct_name)

			# insert members for compound type
			for superVar, dtype in content[struct_name]['Members'].items():

				# standard datatype
				if dtype in self.dtype_map:
					hdf5_dtype = self.dtype_map[dtype]
					if (struct_name == name):
						comp_dec += '\th5_datatype.insertMember("{}", HOFFSET({}Data, {}),{});\n'.format(superVar, struct_name,superVar, hdf5_dtype)
					else:
						comp_dec += '\tmtype_{}.insertMember("{}", HOFFSET({}, {}),{});\n'.format(struct_name, superVar,struct_name, superVar, hdf5_dtype)

				# string datatype
				elif 'string' in dtype:
					if (struct_name == name):
						comp_dec += '\th5_datatype.insertMember("{}", HOFFSET({}Data, {}),vlst_{});\n'.format(superVar, struct_name, superVar, dtype)
					else:
						comp_dec += '\tmtype_{}.insertMember("{}", HOFFSET({}, {}),vlst_{});\n'.format(struct_name, superVar, struct_name, superVar, dtype)

				# array datatype
				elif 'std::array' in dtype:
					st_index = dtype.find('<') + 1
					end_index = dtype.find(',')
					data_type = dtype[st_index:end_index].strip()

					# if the array is made up of standard datatype like int, double etc.
					if data_type in self.a_type_map:
						hdf5_dtype = self.a_type_map[data_type]
						if (struct_name == name):
							comp_dec += '\th5_datatype.insertMember("{}", HOFFSET({}Data, {}),H5Tarray_create({}, 1, {}_array_dim));\n'.format(superVar, struct_name, superVar, hdf5_dtype,superVar)
						else:
							comp_dec += '\tmtype_{}.insertMember("{}", HOFFSET({}, {}),H5Tarray_create({}, 1, {}_array_dim));\n'.format(struct_name, superVar, struct_name, superVar, hdf5_dtype, superVar)

					# otherwise we have an array with a compound type
					else:
						if (struct_name == name):
							comp_dec += '\th5_datatype.insertMember("{}", HOFFSET({}Data, {}),H5Tarray_create(mtype_{}.getId(), 1, {}_array_dim));\n'.format(superVar, struct_name, superVar, data_type,superVar)
						else:
							comp_dec += '\tmtype_{}.insertMember("{}", HOFFSET({}, {}),H5Tarray_create(mtype_{}.getId(), 1, {}_array_dim));\n'.format(struct_name, superVar, struct_name, superVar, data_type, superVar)

				# else it is a compound type
				else:
					if (struct_name == name):
						comp_dec += '\th5_datatype.insertMember("{}", HOFFSET({}Data, {}),mtype_{});\n'.format(superVar, struct_name, superVar, dtype)
					else:
						comp_dec += '\tmtype_{}.insertMember("{}", HOFFSET({}, {}),mtype_{});\n'.format(struct_name, superVar, struct_name, superVar, dtype)



		# set event_count = 0
		end  = "\n\tevent_count = 0;\n}\n"

		return constructor_dec + struct_array_dec + string_dec + comp_dec + end

	def write_hdf5_src(self, name, content):

		include = '#include "H5{}Collection.h"\n'.format(name)

		# declare constructor
		constructor = self.get_constructor(name, content)

		# the functions below are generic and depend only on the name of the dataype
		get_instance = "\n\nH5{}Collection* H5{}Collection::getInstance()\n".format(name, name) + \
						"{\n" + \
					    "\tif (instance == 0)\n" + \
					   "\t\t{\n" + \
					      "\t\t\tinstance = new H5{}Collection();\n".format(name) + \
					   "\t\t}\n\n" + \
					    "\treturn instance;\n" + \
					"}\n\n"

		get_collection = "\n\n{}Collection* H5{}Collection::get_collection(const std::string& name, EventStore* m_store)\n".format(name, name) + \
						"{\n" + \
						"\tconst {}Collection* tmp_coll(nullptr);\n".format(name) + \
						"\tm_store->get<{}Collection>(name, tmp_coll);\n".format(name) + \
						"\t{}Collection* coll = const_cast<{}Collection*>(tmp_coll);\n".format(name, name) + \
						"\treturn coll;\n" + \
						"}\n"

		data_name = name + "Data"
		write_collection = "\n\nvoid H5{}Collection::writeCollection(CollectionBase* c, H5File& file)\n".format(name) +\
							"{\n" + \
							"\tevent_count += 1;\n\n" + \
							"\t{}Collection* info = static_cast<{}Collection*>( c );\n".format(name,name) + \
							"\tvoid* buffer = info->_getBuffer();\n" + \
							"\t{}** data = reinterpret_cast<{}**>(buffer);\n\n".format(data_name, data_name) + \
							"\tconst int SIZE = info->size();\n" + \
							"\tif(SIZE > 0)\n" + \
						 	"\t{\n" + \
							"\t\thsize_t dim[] = {static_cast<hsize_t>(SIZE)};\n" + \
							"\t\tDataSpace space(RANK_{}, dim);\n".format(name.upper()) + \
							"\t\tstring group_name = std::to_string(event_count-1);\n" + \
							'\t\tstring dataset_name = group_name + "/{}";\n\n'.format(name) + \
							"\t\tGroup g;\n" + \
							"\t\tif (pathExists(file.getId(), group_name))\n" + \
						    "\t\t\tg=file.openGroup(group_name.c_str());\n" + \
							"\t\telse\n" + \
							"\t\t\tg=file.createGroup(group_name.c_str());\n\n" + \
							"\t\tDataSet d = file.createDataSet(dataset_name.c_str(), h5_datatype, space);\n" + \
							"\t\t// Write data to file\n" + \
							"\t\td.write(*data, h5_datatype);\n" + \
							"\t}\n"  + \
						 	"}\n"
		content = include + constructor + get_instance + get_collection + write_collection
		filename = "H5{}Collection".format(name) + ".cc"
		self.write_file(filename, content)

	def write_file(self, name,content):

	  #print 'ClassGenerator write_file TRIGGERED\n'

	  #dispatch headers to header dir, the rest to /src
	  # fullname = os.path.join(self.install_dir,self.package_name,name)
	  if name.endswith("h"):
		fullname = os.path.join(self.install_dir,self.package_name,name)
	  else:
		#print 'HDF5 file here'
		fullname = os.path.join(self.install_dir,"src",name)
	  if not self.dryrun:
		#print 'HDF5 file here dryrun'
		#print('fullname = {}'.format(fullname))
		open(fullname, "w").write(content)
		if self.clang_format:
		  subprocess.call(self.clang_format + [fullname])

	def get_includes(self, name):
		result = "#include <iostream>\n" + \
				  "#include <typeindex>\n" + \
				  '#include "H5Cpp.h"\n'  + \
				  '#include "{}Data.h"\n'.format(name) + \
				  '#include "{}Collection.h"'.format(name)

		result += "\n\n"

		result += '#include "podio/EventStore.h"\n' + \
				   '#include "podio/CollectionBase.h"\n' + \
				   '#include "podio/HDF5Writer.h"\n\n\n'

		return result

	def get_namespace(self, name):
		result = "using namespace H5;\n" + \
		 		 "using namespace std;\n" + \
				 "using namespace podio;\n"

		for x in self.namespace[name]:
			if x !=	'std':
				result += "using namespace {};\n".format(x)

		return result

	def remove_namespace(self, content):
		new_content = copy.deepcopy(content)
		#pprint(content)

		for varName in content:
			new_key = varName

			if "::" in varName:
				last_index = varName.rindex("::")
				new_key = varName[last_index+2 : ]
				members = content[varName]
				new_content.pop(varName, 'None')
				new_content[new_key] = members

			#print("--------------------------------")
			#print("NEW KEY = ", new_key)
			#print 'BEFORE'
			#pprint(new_content)
			#print('--------------------------------')

			for name, dtype in content[varName]['Members'].items():
				new_dtype = dtype
				if "::" in dtype and "std::array" not in dtype:
					last_index = dtype.rindex("::")
					new_dtype = dtype[last_index+2 : ]

				if "::" in dtype and "std::array" in dtype:
					shift_dtype = dtype[dtype.index("std::array")+9 : ]
					last_index = 0
					if "::" in shift_dtype:
						last_index = shift_dtype.rindex("::")

					new_dtype = "std::array<" + shift_dtype[last_index+2 : ]

				new_content[new_key]['Members'][name] = new_dtype

			#print 'AFTER'
			#pprint (new_content)


		return new_content

	def strip_namespace(self,name):
		if "::" in name:
			last_index = name.rindex("::")
			return name[last_index+2 : ]
		return name
















##########################
if __name__ == "__main__":

    from optparse import OptionParser

    usage = """usage: %prog [options] <description.yaml> <targetdir> <packagename>
    Given a <description.yaml>
    it creates data classes
    and a LinkDef.h file in
    the specified <targetdir>:
      <packagename>/*.h
      src/*.cc"""

    parser = OptionParser(usage)
    parser.add_option("-q", "--quiet",
                    action="store_false", dest="verbose", default=True,
                    help="Don't write a report to screen")
    parser.add_option("-d", "--dryrun",
                    action="store_true", dest="dryrun", default=False,
                    help="Do not actually write datamodel files")
    parser.add_option("-c", "--clangformat", dest="clangformat",
                    action="store_true", default=False,
                    help="Apply clang-format when generating code (with -style=file)")
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

    #print("Something happened", install_path, project)


    if not os.path.exists( directory ):
      os.makedirs(directory)

    gen = ClassGenerator(args[0], args[1], args[2], verbose=options.verbose, dryrun=options.dryrun)
    gen.configure_clang_format(options.clangformat)
    gen.process()
    for warning in gen.warnings:
      print (warning)
