#include "H5ExampleWithARelationCollection.h"
H5ExampleWithARelationCollection::H5ExampleWithARelationCollection()
{

	h5_datatype = CompType(sizeof(ExampleWithARelationData));
	h5_datatype.insertMember("number", HOFFSET(ExampleWithARelationData, number),PredType::NATIVE_FLOAT);
	h5_datatype.insertMember("refs_begin", HOFFSET(ExampleWithARelationData, refs_begin),PredType::NATIVE_UINT);
	h5_datatype.insertMember("refs_end", HOFFSET(ExampleWithARelationData, refs_end),PredType::NATIVE_UINT);

	event_count = 0;
}


H5ExampleWithARelationCollection* H5ExampleWithARelationCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5ExampleWithARelationCollection();
		}

	return instance;
}



ExampleWithARelationCollection* H5ExampleWithARelationCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const ExampleWithARelationCollection* tmp_coll(nullptr);
	m_store->get<ExampleWithARelationCollection>(name, tmp_coll);
	ExampleWithARelationCollection* coll = const_cast<ExampleWithARelationCollection*>(tmp_coll);
	return coll;
}


void H5ExampleWithARelationCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	ExampleWithARelationCollection* info = static_cast<ExampleWithARelationCollection*>( c );
	void* buffer = info->_getBuffer();
	ExampleWithARelationData** data = reinterpret_cast<ExampleWithARelationData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EXAMPLEWITHARELATION, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/ExampleWithARelation";

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
