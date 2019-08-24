#ifndef H5EXAMPLEREFERENCINGTYPEDATA_H
#define H5EXAMPLEREFERENCINGTYPEDATA_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleReferencingTypeData.h"
#include "ExampleReferencingTypeCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"


using namespace H5;
using namespace std;
using namespace podio;
const H5std_string EXAMPLEREFERENCINGTYPEDATA("ExampleReferencingTypeData");
const int RANK_EXAMPLEREFERENCINGTYPE = 1;


class H5ExampleReferencingTypeCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned int event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5ExampleReferencingTypeCollection* getInstance();
		ExampleReferencingTypeCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5ExampleReferencingTypeCollection* instance;
		H5ExampleReferencingTypeCollection();
};

/* Initially the instance is NULL */
H5ExampleReferencingTypeCollection* H5ExampleReferencingTypeCollection::instance = 0;


#endif