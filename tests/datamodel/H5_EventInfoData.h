#ifndef H5_EVENTINFODATA_H
#define H5_EVENTINFODATA_H

#include <iostream>
#include "H5Cpp.h"
#include "EventInfoData.h"
#include "EventInfoCollection.h"


#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"

using namespace H5;
using namespace std;

const H5std_string EVENT_INFO_DATA("EventInfoData");
const int RANK = 1;

class H5_EventInfoData
{
	public:
		CompType h5_datatype;
		DSetCreatPropList cparms;
		DataSet dataset_eventInfo;		
		hsize_t size[1];
		hsize_t offset[1];
		int flag; // if flag = 0 then we are writing the dataset for the first time
		
	public:
		H5_EventInfoData();
		void writeH5(H5File& file, EventInfoCollection& info);
	


};

H5_EventInfoData::H5_EventInfoData() 
{
	h5_datatype = CompType(sizeof(EventInfoData));
	h5_datatype.insertMember("Number", HOFFSET(EventInfoData, Number), H5::PredType::NATIVE_INT);
	hsize_t chunk_dims[2] = {1, 5};
	cparms.setChunk(1, chunk_dims); 
	flag = 0;
	

}





#endif
