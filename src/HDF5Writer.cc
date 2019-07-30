// HDF5 specific includes
#include "H5Cpp.h"

// podio specific includes
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/HDF5Writer.h"

using namespace H5;
namespace podio{

	HDF5Writer::HDF5Writer(){;}

	void HDF5Writer::writeCollection(CollectionBase* c, H5File& file){;}

	std::type_index HDF5Writer::get_typeindex(){;}


	HDF5Writer::HDF5Writer(const H5std_string FILE_NAME, EventStore* store):
		m_filename(FILE_NAME),
		m_store(store)
		{
			 // create the file once
				H5File file(m_filename, H5F_ACC_TRUNC);
			std::cout<< "HDF5_WRITER object created" << std::endl;
		}

	void HDF5Writer::writeEvent(std::map<std::type_index, HDF5Writer*> h5map)
	{
		// read and write access to the file
		H5File file(m_filename, H5F_ACC_RDWR);
		unsigned i = 0;
		for(auto& col : m_storedCollections)
			{
				col->prepareForWrite();
				const std::string &name = m_collectionNames[i++];
				std::cout <<"Writing collection "<<name<<std::endl;
				auto type = Fmap.at(name);
				HDF5Writer* wrt = h5map.at(type);
				wrt->writeCollection(col, file);

			}
	}


}
