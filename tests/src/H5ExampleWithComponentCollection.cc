#include "H5ExampleWithComponentCollection.h"
H5ExampleWithComponentCollection::H5ExampleWithComponentCollection()
{
	hsize_t p_array_dim[] = {4};

	CompType mtype_SimpleStruct(sizeof(SimpleStruct));
	mtype_SimpleStruct.insertMember("y", HOFFSET(SimpleStruct, y),PredType::NATIVE_INT);
	mtype_SimpleStruct.insertMember("x", HOFFSET(SimpleStruct, x),PredType::NATIVE_INT);
	mtype_SimpleStruct.insertMember("z", HOFFSET(SimpleStruct, z),PredType::NATIVE_INT);
	mtype_SimpleStruct.insertMember("p", HOFFSET(SimpleStruct, p),H5Tarray_create(H5T_NATIVE_INT, 1, p_array_dim));

	CompType mtype_NotSoSimpleStruct(sizeof(NotSoSimpleStruct));
	mtype_NotSoSimpleStruct.insertMember("data", HOFFSET(NotSoSimpleStruct, data),mtype_SimpleStruct);

	h5_datatype = CompType(sizeof(ExampleWithComponentData));
	h5_datatype.insertMember("component", HOFFSET(ExampleWithComponentData, component),mtype_NotSoSimpleStruct);

	event_count = 0;
}


H5ExampleWithComponentCollection* H5ExampleWithComponentCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5ExampleWithComponentCollection();
		}

	return instance;
}



ExampleWithComponentCollection* H5ExampleWithComponentCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const ExampleWithComponentCollection* tmp_coll(nullptr);
	m_store->get<ExampleWithComponentCollection>(name, tmp_coll);
	ExampleWithComponentCollection* coll = const_cast<ExampleWithComponentCollection*>(tmp_coll);
	return coll;
}


void H5ExampleWithComponentCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	ExampleWithComponentCollection* info = static_cast<ExampleWithComponentCollection*>( c );
	void* buffer = info->_getBuffer();
	ExampleWithComponentData** data = reinterpret_cast<ExampleWithComponentData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EXAMPLEWITHCOMPONENT, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/ExampleWithComponent";

		Group g;
		if (pathExists(file.getId(), group_name))
			g=file.openGroup(group_name.c_str());
		else
			g=file.createGroup(group_name.c_str());

		DataSet d = file.createDataSet(dataset_name.c_str(), h5_datatype, space);
		// Write data to file
		d.write(*data, h5_datatype);
	}
}
