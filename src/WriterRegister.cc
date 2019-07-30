// HDF5 specific includes
#include "H5Cpp.h"

// podio specific includes
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/HDF5Writer.h"
#include "podio/WriterRegister.h"

namespace podio
{

	WriterRegister::WriterRegister()
	{
		;
	}


	WriterRegister* WriterRegister::getInstance()
	{

		std::cout << "Getting WriterRegister instance" << std::endl;

		if (instance == 0)
		{
		    instance = new WriterRegister();
		}

		return instance;
	}

	void WriterRegister::register_writer(HDF5Writer* writer)
	{
		// register EventInfoData
		auto type = writer->get_typeindex();
		h5map[type] = writer;

	}

}
