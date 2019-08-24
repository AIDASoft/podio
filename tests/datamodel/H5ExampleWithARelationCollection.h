#ifndef H5EXAMPLEWITHARELATIONDATA_H
#define H5EXAMPLEWITHARELATIONDATA_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleWithARelationData.h"
#include "ExampleWithARelationCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"


using namespace H5;
using namespace std;
using namespace podio;
using namespace ex;
const H5std_string EXAMPLEWITHARELATIONDATA("ExampleWithARelationData");
const int RANK_EXAMPLEWITHARELATION = 1;


class H5ExampleWithARelationCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned int event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5ExampleWithARelationCollection* getInstance();
		ExampleWithARelationCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5ExampleWithARelationCollection* instance;
		H5ExampleWithARelationCollection();
};

/* Initially the instance is NULL */
H5ExampleWithARelationCollection* H5ExampleWithARelationCollection::instance = 0;


#endif