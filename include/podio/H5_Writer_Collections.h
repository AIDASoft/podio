#ifndef H5_WRITER_COLLECTIONS_H
#define H5_WRITER_COLLECTIONS_H

// STL
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <utility>

// podio specification
#include "podio/EventStore.h"
#include "podio/CollectionBase.h"

// HDF5 specific includes
#include "H5Cpp.h"

using namespace H5;

namespace podio
{

	class CollectionBase;

class H5_Writer_Collections
{
	public:
		H5_Writer_Collections();
		virtual void writeCollection(CollectionBase* col, H5File& file);
		virtual CollectionBase* get_collection(const std::string& name, EventStore* m_store);

};


}
#endif
