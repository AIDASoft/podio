#ifndef H5EXAMPLEHITDATA_H
#define H5EXAMPLEHITDATA_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleHitData.h"
#include "ExampleHitCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"


using namespace H5;
using namespace std;
using namespace podio;
const H5std_string EXAMPLEHITDATA("ExampleHitData");
const int RANK_EXAMPLEHIT = 1;


class H5ExampleHitCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned int event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5ExampleHitCollection* getInstance();
		ExampleHitCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5ExampleHitCollection* instance;
		H5ExampleHitCollection();
};

/* Initially the instance is NULL */
H5ExampleHitCollection* H5ExampleHitCollection::instance = 0;


#endif