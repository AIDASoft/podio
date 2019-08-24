#include "H5ExampleMCCollection.h"
H5ExampleMCCollection::H5ExampleMCCollection()
{

	h5_datatype = CompType(sizeof(ExampleMCData));
	h5_datatype.insertMember("energy", HOFFSET(ExampleMCData, energy),PredType::NATIVE_DOUBLE);
	h5_datatype.insertMember("PDG", HOFFSET(ExampleMCData, PDG),PredType::NATIVE_INT);
	h5_datatype.insertMember("parents_begin", HOFFSET(ExampleMCData, parents_begin),PredType::NATIVE_UINT);
	h5_datatype.insertMember("parents_end", HOFFSET(ExampleMCData, parents_end),PredType::NATIVE_UINT);
	h5_datatype.insertMember("daughters_begin", HOFFSET(ExampleMCData, daughters_begin),PredType::NATIVE_UINT);
	h5_datatype.insertMember("daughters_end", HOFFSET(ExampleMCData, daughters_end),PredType::NATIVE_UINT);

	event_count = 0;
}


H5ExampleMCCollection* H5ExampleMCCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5ExampleMCCollection();
		}

	return instance;
}



ExampleMCCollection* H5ExampleMCCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const ExampleMCCollection* tmp_coll(nullptr);
	m_store->get<ExampleMCCollection>(name, tmp_coll);
	ExampleMCCollection* coll = const_cast<ExampleMCCollection*>(tmp_coll);
	return coll;
}


void H5ExampleMCCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	ExampleMCCollection* info = static_cast<ExampleMCCollection*>( c );
	void* buffer = info->_getBuffer();
	ExampleMCData** data = reinterpret_cast<ExampleMCData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EXAMPLEMC, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/ExampleMC";

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
