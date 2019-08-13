#include "H5_Writer_EventInfoCollection.h"

H5_Writer_EventInfoCollection::H5_Writer_EventInfoCollection()
{

	h5_datatype = CompType(sizeof(EventInfoData));
	h5_datatype.insertMember("Number", HOFFSET(EventInfoData, Number),PredType::NATIVE_INT);

	hsize_t chunk_dims[2] = {1, 5};
	cparms.setChunk(1, chunk_dims);
	flag = 0;

}

H5_Writer_EventInfoCollection* H5_Writer_EventInfoCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5_Writer_EventInfoCollection();
		}

	return instance;
}



EventInfoCollection* H5_Writer_EventInfoCollection::get_collection(const std::string& name, EventStore* m_store)
{
	std::cout << "returning the typeid of EventInfoCollection" <<std::endl;
	const EventInfoCollection* tmp_coll(nullptr);
	m_store->get<EventInfoCollection>(name, tmp_coll);
	EventInfoCollection* coll = const_cast<EventInfoCollection*>(tmp_coll);
	return coll;

}


void H5_Writer_EventInfoCollection::writeCollection(CollectionBase* c, H5File& file)
{
	EventInfoCollection* info = static_cast<EventInfoCollection*>( c );
	void* buffer_1 = info->_getBuffer();
	EventInfoData** data_1 = reinterpret_cast<EventInfoData**>(buffer_1);

	if(flag == 0)
	{
		size[0] = info->size();
		/*
		* Initialize the data space with unlimited dimensions.
		*/
		hsize_t dims[1]  = {info->size()};
		hsize_t maxdims[1] = {H5S_UNLIMITED};
		DataSpace mspace1( RANK_EventInfoCollection, dims, maxdims);

		dataset_EventInfo = file.createDataSet(EVENTINFODATA, h5_datatype, mspace1, cparms);
		DataSpace fspace1 = dataset_EventInfo.getSpace();
		offset[0] = 0;
		hsize_t dims1[1] = {info->size()};            /* data1 dimensions */
		fspace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

		dataset_EventInfo.extend(size);

		// Write data to file
		dataset_EventInfo.write(*data_1, h5_datatype, mspace1, fspace1);

		// we have written once so set flag = 1
		flag = 1;

	}

	else
	{
		// extend dataset_EventInfo
		hsize_t dim2[1] = {info->size()};

		offset[0] = size[0];
		size[0] += info->size();
		dataset_EventInfo.extend( size );

		// select hyperslab
		DataSpace fspace2 = dataset_EventInfo.getSpace();
		fspace2.selectHyperslab(H5S_SELECT_SET, dim2, offset );

		DataSpace mspace2( RANK_EventInfoCollection, dim2 );

		// Write data to file
		dataset_EventInfo.write(*data_1, h5_datatype, mspace2, fspace2);

	}

}
