#include "H5ExampleWithStringCollection.h"
H5ExampleWithStringCollection::H5ExampleWithStringCollection()
{
	StrType vlst_string(0, H5T_VARIABLE);

	h5_datatype = CompType(sizeof(ExampleWithStringData));
	h5_datatype.insertMember("theString", HOFFSET(ExampleWithStringData, theString),vlst_string);

	event_count = 0;
}


H5ExampleWithStringCollection* H5ExampleWithStringCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5ExampleWithStringCollection();
		}

	return instance;
}



ExampleWithStringCollection* H5ExampleWithStringCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const ExampleWithStringCollection* tmp_coll(nullptr);
	m_store->get<ExampleWithStringCollection>(name, tmp_coll);
	ExampleWithStringCollection* coll = const_cast<ExampleWithStringCollection*>(tmp_coll);
	return coll;
}


void H5ExampleWithStringCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	ExampleWithStringCollection* info = static_cast<ExampleWithStringCollection*>( c );
	void* buffer = info->_getBuffer();
	ExampleWithStringData** data = reinterpret_cast<ExampleWithStringData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EXAMPLEWITHSTRING, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/ExampleWithString";

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
