#include "H5ExampleClusterCollection.h"
H5ExampleClusterCollection::H5ExampleClusterCollection()
{

	h5_datatype = CompType(sizeof(ExampleClusterData));
	h5_datatype.insertMember("energy", HOFFSET(ExampleClusterData, energy),PredType::NATIVE_DOUBLE);
	h5_datatype.insertMember("Hits_begin", HOFFSET(ExampleClusterData, Hits_begin),PredType::NATIVE_UINT);
	h5_datatype.insertMember("Hits_end", HOFFSET(ExampleClusterData, Hits_end),PredType::NATIVE_UINT);
	h5_datatype.insertMember("Clusters_begin", HOFFSET(ExampleClusterData, Clusters_begin),PredType::NATIVE_UINT);
	h5_datatype.insertMember("Clusters_end", HOFFSET(ExampleClusterData, Clusters_end),PredType::NATIVE_UINT);

	event_count = 0;
}


H5ExampleClusterCollection* H5ExampleClusterCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5ExampleClusterCollection();
		}

	return instance;
}



ExampleClusterCollection* H5ExampleClusterCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const ExampleClusterCollection* tmp_coll(nullptr);
	m_store->get<ExampleClusterCollection>(name, tmp_coll);
	ExampleClusterCollection* coll = const_cast<ExampleClusterCollection*>(tmp_coll);
	return coll;
}


void H5ExampleClusterCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	ExampleClusterCollection* info = static_cast<ExampleClusterCollection*>( c );
	void* buffer = info->_getBuffer();
	ExampleClusterData** data = reinterpret_cast<ExampleClusterData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EXAMPLECLUSTER, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/ExampleCluster";

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
