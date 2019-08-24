#include "H5ExampleWithNamespaceCollection.h"
H5ExampleWithNamespaceCollection::H5ExampleWithNamespaceCollection()
{

	CompType mtype_NamespaceStruct(sizeof(NamespaceStruct));
	mtype_NamespaceStruct.insertMember("y", HOFFSET(NamespaceStruct, y),PredType::NATIVE_INT);
	mtype_NamespaceStruct.insertMember("x", HOFFSET(NamespaceStruct, x),PredType::NATIVE_INT);

	h5_datatype = CompType(sizeof(ExampleWithNamespaceData));
	h5_datatype.insertMember("data", HOFFSET(ExampleWithNamespaceData, data),mtype_NamespaceStruct);

	event_count = 0;
}


H5ExampleWithNamespaceCollection* H5ExampleWithNamespaceCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5ExampleWithNamespaceCollection();
		}

	return instance;
}



ExampleWithNamespaceCollection* H5ExampleWithNamespaceCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const ExampleWithNamespaceCollection* tmp_coll(nullptr);
	m_store->get<ExampleWithNamespaceCollection>(name, tmp_coll);
	ExampleWithNamespaceCollection* coll = const_cast<ExampleWithNamespaceCollection*>(tmp_coll);
	return coll;
}


void H5ExampleWithNamespaceCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	ExampleWithNamespaceCollection* info = static_cast<ExampleWithNamespaceCollection*>( c );
	void* buffer = info->_getBuffer();
	ExampleWithNamespaceData** data = reinterpret_cast<ExampleWithNamespaceData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EXAMPLEWITHNAMESPACE, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/ExampleWithNamespace";

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
