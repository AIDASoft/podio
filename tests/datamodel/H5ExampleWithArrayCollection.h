#ifndef H5EXAMPLEWITHARRAYDATA_H
#define H5EXAMPLEWITHARRAYDATA_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleWithArrayData.h"
#include "ExampleWithArrayCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"


using namespace H5;
using namespace std;
using namespace podio;
using namespace ex2;
const H5std_string EXAMPLEWITHARRAYDATA("ExampleWithArrayData");
const int RANK_EXAMPLEWITHARRAY = 1;


class H5ExampleWithArrayCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned int event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5ExampleWithArrayCollection* getInstance();
		ExampleWithArrayCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5ExampleWithArrayCollection* instance;
		H5ExampleWithArrayCollection();
};

/* Initially the instance is NULL */
H5ExampleWithArrayCollection* H5ExampleWithArrayCollection::instance = 0;


#endif