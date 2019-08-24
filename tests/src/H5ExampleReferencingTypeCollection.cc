#include "H5ExampleReferencingTypeCollection.h"
H5ExampleReferencingTypeCollection::H5ExampleReferencingTypeCollection()
{

	h5_datatype = CompType(sizeof(ExampleReferencingTypeData));
	h5_datatype.insertMember("Clusters_begin", HOFFSET(ExampleReferencingTypeData, Clusters_begin),PredType::NATIVE_UINT);
	h5_datatype.insertMember("Clusters_end", HOFFSET(ExampleReferencingTypeData, Clusters_end),PredType::NATIVE_UINT);
	h5_datatype.insertMember("Refs_begin", HOFFSET(ExampleReferencingTypeData, Refs_begin),PredType::NATIVE_UINT);
	h5_datatype.insertMember("Refs_end", HOFFSET(ExampleReferencingTypeData, Refs_end),PredType::NATIVE_UINT);

	event_count = 0;
}


H5ExampleReferencingTypeCollection* H5ExampleReferencingTypeCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5ExampleReferencingTypeCollection();
		}

	return instance;
}



ExampleReferencingTypeCollection* H5ExampleReferencingTypeCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const ExampleReferencingTypeCollection* tmp_coll(nullptr);
	m_store->get<ExampleReferencingTypeCollection>(name, tmp_coll);
	ExampleReferencingTypeCollection* coll = const_cast<ExampleReferencingTypeCollection*>(tmp_coll);
	return coll;
}


void H5ExampleReferencingTypeCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	ExampleReferencingTypeCollection* info = static_cast<ExampleReferencingTypeCollection*>( c );
	void* buffer = info->_getBuffer();
	ExampleReferencingTypeData** data = reinterpret_cast<ExampleReferencingTypeData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EXAMPLEREFERENCINGTYPE, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/ExampleReferencingType";

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
