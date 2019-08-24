#ifndef H5EXAMPLEWITHVECTORMEMBERDATA_H
#define H5EXAMPLEWITHVECTORMEMBERDATA_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleWithVectorMemberData.h"
#include "ExampleWithVectorMemberCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"


using namespace H5;
using namespace std;
using namespace podio;
const H5std_string EXAMPLEWITHVECTORMEMBERDATA("ExampleWithVectorMemberData");
const int RANK_EXAMPLEWITHVECTORMEMBER = 1;


class H5ExampleWithVectorMemberCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned int event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5ExampleWithVectorMemberCollection* getInstance();
		ExampleWithVectorMemberCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5ExampleWithVectorMemberCollection* instance;
		H5ExampleWithVectorMemberCollection();
};

/* Initially the instance is NULL */
H5ExampleWithVectorMemberCollection* H5ExampleWithVectorMemberCollection::instance = 0;


#endif