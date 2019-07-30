#ifndef H5_EVENTINFODATA_H
#define H5_EVENTINFODATA_H

#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "EventInfoData.h"
#include "EventInfoCollection.h"


#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"

using namespace H5;
using namespace std;
using namespace podio;

const H5std_string EVENT_INFO_DATA("EventInfoData");
const int RANK = 1;


class H5_EventInfoData : public podio::HDF5Writer
{
	public:
		CompType h5_datatype;
		DSetCreatPropList cparms;
		DataSet dataset_eventInfo;
		hsize_t size[1];
		hsize_t offset[1];
		int flag; // if flag = 0 then we are writing the dataset for the first time

	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5_EventInfoData* getInstance();
		std::type_index get_typeindex();

	private:
		static H5_EventInfoData* instance;
		H5_EventInfoData();
};

/* Initially the instance is NULL */
H5_EventInfoData* H5_EventInfoData::instance = 0;


#endif
