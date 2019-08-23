#ifndef H5EVENTINFODATA_H
#define H5EVENTINFODATA_H


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
const H5std_string EVENTINFODATA("EventInfoData");
const int RANK_EVENTINFO = 1;


class H5EventInfoCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5EventInfoCollection* getInstance();
		EventInfoCollection* get_collection(const std::string& name, EventStore* m_store);
		void readCollection(unsigned event_number, H5File& file, EventStore& m_store);

	private:
		static H5EventInfoCollection* instance;
		H5EventInfoCollection();
};

/* Initially the instance is NULL */
H5EventInfoCollection* H5EventInfoCollection::instance = 0;


#endif
