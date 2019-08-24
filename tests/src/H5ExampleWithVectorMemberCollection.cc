#include "H5ExampleWithVectorMemberCollection.h"
H5ExampleWithVectorMemberCollection::H5ExampleWithVectorMemberCollection()
{

	h5_datatype = CompType(sizeof(ExampleWithVectorMemberData));
	h5_datatype.insertMember("count_begin", HOFFSET(ExampleWithVectorMemberData, count_begin),PredType::NATIVE_UINT);
	h5_datatype.insertMember("count_end", HOFFSET(ExampleWithVectorMemberData, count_end),PredType::NATIVE_UINT);

	event_count = 0;
}


H5ExampleWithVectorMemberCollection* H5ExampleWithVectorMemberCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5ExampleWithVectorMemberCollection();
		}

	return instance;
}



ExampleWithVectorMemberCollection* H5ExampleWithVectorMemberCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const ExampleWithVectorMemberCollection* tmp_coll(nullptr);
	m_store->get<ExampleWithVectorMemberCollection>(name, tmp_coll);
	ExampleWithVectorMemberCollection* coll = const_cast<ExampleWithVectorMemberCollection*>(tmp_coll);
	return coll;
}


void H5ExampleWithVectorMemberCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	ExampleWithVectorMemberCollection* info = static_cast<ExampleWithVectorMemberCollection*>( c );
	void* buffer = info->_getBuffer();
	ExampleWithVectorMemberData** data = reinterpret_cast<ExampleWithVectorMemberData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EXAMPLEWITHVECTORMEMBER, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/ExampleWithVectorMember";

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
