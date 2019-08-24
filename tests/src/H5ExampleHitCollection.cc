#include "H5ExampleHitCollection.h"
H5ExampleHitCollection::H5ExampleHitCollection()
{

	h5_datatype = CompType(sizeof(ExampleHitData));
	h5_datatype.insertMember("x", HOFFSET(ExampleHitData, x),PredType::NATIVE_DOUBLE);
	h5_datatype.insertMember("y", HOFFSET(ExampleHitData, y),PredType::NATIVE_DOUBLE);
	h5_datatype.insertMember("z", HOFFSET(ExampleHitData, z),PredType::NATIVE_DOUBLE);
	h5_datatype.insertMember("energy", HOFFSET(ExampleHitData, energy),PredType::NATIVE_DOUBLE);

	event_count = 0;
}


H5ExampleHitCollection* H5ExampleHitCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5ExampleHitCollection();
		}

	return instance;
}



ExampleHitCollection* H5ExampleHitCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const ExampleHitCollection* tmp_coll(nullptr);
	m_store->get<ExampleHitCollection>(name, tmp_coll);
	ExampleHitCollection* coll = const_cast<ExampleHitCollection*>(tmp_coll);
	return coll;
}


void H5ExampleHitCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	ExampleHitCollection* info = static_cast<ExampleHitCollection*>( c );
	void* buffer = info->_getBuffer();
	ExampleHitData** data = reinterpret_cast<ExampleHitData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EXAMPLEHIT, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/ExampleHit";

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
