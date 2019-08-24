#ifndef H5EXAMPLEWITHCOMPONENTDATA_H
#define H5EXAMPLEWITHCOMPONENTDATA_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleWithComponentData.h"
#include "ExampleWithComponentCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"


using namespace H5;
using namespace std;
using namespace podio;
const H5std_string EXAMPLEWITHCOMPONENTDATA("ExampleWithComponentData");
const int RANK_EXAMPLEWITHCOMPONENT = 1;


class H5ExampleWithComponentCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned int event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5ExampleWithComponentCollection* getInstance();
		ExampleWithComponentCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5ExampleWithComponentCollection* instance;
		H5ExampleWithComponentCollection();
};

/* Initially the instance is NULL */
H5ExampleWithComponentCollection* H5ExampleWithComponentCollection::instance = 0;


#endif