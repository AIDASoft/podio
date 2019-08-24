#include "H5EventInfoCollection.h"
H5EventInfoCollection::H5EventInfoCollection()
{

	h5_datatype = CompType(sizeof(EventInfoData));
	h5_datatype.insertMember("Number", HOFFSET(EventInfoData, Number),PredType::NATIVE_INT);

	event_count = 0;
}


H5EventInfoCollection* H5EventInfoCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5EventInfoCollection();
		}

	return instance;
}



EventInfoCollection* H5EventInfoCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const EventInfoCollection* tmp_coll(nullptr);
	m_store->get<EventInfoCollection>(name, tmp_coll);
	EventInfoCollection* coll = const_cast<EventInfoCollection*>(tmp_coll);
	return coll;
}


void H5EventInfoCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	EventInfoCollection* info = static_cast<EventInfoCollection*>( c );
	void* buffer = info->_getBuffer();
	EventInfoData** data = reinterpret_cast<EventInfoData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EVENTINFO, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/EventInfo";

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
