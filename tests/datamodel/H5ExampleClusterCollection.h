#ifndef H5EXAMPLECLUSTERDATA_H
#define H5EXAMPLECLUSTERDATA_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleClusterData.h"
#include "ExampleClusterCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"


using namespace H5;
using namespace std;
using namespace podio;
const H5std_string EXAMPLECLUSTERDATA("ExampleClusterData");
const int RANK_EXAMPLECLUSTER = 1;


class H5ExampleClusterCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned int event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5ExampleClusterCollection* getInstance();
		ExampleClusterCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5ExampleClusterCollection* instance;
		H5ExampleClusterCollection();
};

/* Initially the instance is NULL */
H5ExampleClusterCollection* H5ExampleClusterCollection::instance = 0;


#endif