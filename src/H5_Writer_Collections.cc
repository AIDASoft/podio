// HDF5 specific includes
#include "H5Cpp.h"

// podio specific includes
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/HDF5Writer.h"

using namespace H5;
namespace podio{

	H5_Writer_Collections::H5_Writer_Collections(){;}

	void H5_Writer_Collections::writeCollection(CollectionBase* c, H5File& file){;}

	CollectionBase* H5_Writer_Collections::get_collection(const std::string& name, EventStore* m_store){;}
}
