#ifndef H5EXAMPLEMCDATA_H
#define H5EXAMPLEMCDATA_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleMCData.h"
#include "ExampleMCCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"


using namespace H5;
using namespace std;
using namespace podio;
const H5std_string EXAMPLEMCDATA("ExampleMCData");
const int RANK_EXAMPLEMC = 1;


class H5ExampleMCCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned int event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5ExampleMCCollection* getInstance();
		ExampleMCCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5ExampleMCCollection* instance;
		H5ExampleMCCollection();
};

/* Initially the instance is NULL */
H5ExampleMCCollection* H5ExampleMCCollection::instance = 0;


#endif