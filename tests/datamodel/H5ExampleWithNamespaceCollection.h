#ifndef H5EXAMPLEWITHNAMESPACEDATA_H
#define H5EXAMPLEWITHNAMESPACEDATA_H


#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleWithNamespaceData.h"
#include "ExampleWithNamespaceCollection.h"

#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"


using namespace H5;
using namespace std;
using namespace podio;
using namespace ex2;
using namespace ex;
const H5std_string EXAMPLEWITHNAMESPACEDATA("ExampleWithNamespaceData");
const int RANK_EXAMPLEWITHNAMESPACE = 1;


class H5ExampleWithNamespaceCollection : public podio::H5Collections
{
	public:
		CompType h5_datatype;
		unsigned int event_count;


	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5ExampleWithNamespaceCollection* getInstance();
		ExampleWithNamespaceCollection* get_collection(const std::string& name, EventStore* m_store);


	private:
		static H5ExampleWithNamespaceCollection* instance;
		H5ExampleWithNamespaceCollection();
};

/* Initially the instance is NULL */
H5ExampleWithNamespaceCollection* H5ExampleWithNamespaceCollection::instance = 0;


#endif