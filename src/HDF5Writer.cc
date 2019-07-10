// HDF5 specific includes
#include "H5Cpp.h"

// podio specific includes
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/HDF5Writer.h"

using namespace H5;
namespace podio{
	HDF5Writer::HDF5Writer(const H5std_string FILE_NAME, EventStore* store):
		m_filename(FILE_NAME),
		m_store(store)
	{
		// Create a file
		H5File file(FILE_NAME, H5F_ACC_TRUNC);
	}

	HDF5Writer::~HDF5Writer()
	{;}

	void HDF5Writer::writeEvent()
	{
	unsigned i = 0;
	for(auto& col : m_storedCollections)
		{
		col->prepareForWrite();
		const std::string &name = m_collectionNames[i++];
		std::cout<<"Writing collection "<<name<<std::endl;
		_ColWriterBase* wrt = m_map[ name ];
		wrt->writeCollection(col); 
		}	
	}
}
