#ifndef H5_WRITER_EVENTINFOCOLLECTION_H
#define H5_WRITER_EVENTINFOCOLLECTION_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "EventInfoData.h"
#include "EventInfoCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/H5_Writer_Collections.h"


using namespace H5;
using namespace std;
using namespace podio;
const H5std_string EVENTINFODATA("EventInfoData");
const int RANK_EventInfoCollection = 1;


class H5_Writer_EventInfoCollection : public podio::H5_Writer_Collections
{
	public:
		CompType h5_datatype;
		DSetCreatPropList cparms;
		DataSet dataset_EventInfo;
		hsize_t size[1];
		hsize_t offset[1];
		int flag; // if flag = 0 then we are writing the dataset for the first time

	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5_Writer_EventInfoCollection* getInstance();
		EventInfoCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5_Writer_EventInfoCollection* instance;
		H5_Writer_EventInfoCollection();
};

/* Initially the instance is NULL */
H5_Writer_EventInfoCollection* H5_Writer_EventInfoCollection::instance = 0;


#endif
