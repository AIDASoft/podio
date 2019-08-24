#ifndef H5EXAMPLEWITHSTRINGDATA_H
#define H5EXAMPLEWITHSTRINGDATA_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleWithStringData.h"
#include "ExampleWithStringCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"


using namespace H5;
using namespace std;
using namespace podio;
const H5std_string EXAMPLEWITHSTRINGDATA("ExampleWithStringData");
const int RANK_EXAMPLEWITHSTRING = 1;


class H5ExampleWithStringCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned int event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5ExampleWithStringCollection* getInstance();
		ExampleWithStringCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5ExampleWithStringCollection* instance;
		H5ExampleWithStringCollection();
};

/* Initially the instance is NULL */
H5ExampleWithStringCollection* H5ExampleWithStringCollection::instance = 0;


#endif