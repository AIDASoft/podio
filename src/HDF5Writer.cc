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
			 // create the file once
				H5File file(m_filename, H5F_ACC_TRUNC);
				std::cout<< "HDF5_WRITER object created" << std::endl;
		}

	void HDF5Writer::writeEvent()
	{
		// read and write access to the file
		H5File file(m_filename, H5F_ACC_RDWR);
		unsigned i = 0;

		for(auto& col : m_storedCollections)
			{
				const std::string &name = m_collectionNames[i++];
				std::cout <<"Writing collection "<<name<<std::endl;
				H5Collections* wrt = Fmap.at(name);
				col->prepareForWrite();
				wrt->writeCollection(col, file);

			}
	}


}
