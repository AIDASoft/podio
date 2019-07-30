#ifndef WRITER_REGISTER_H
#define WRITER_REGISTER_H

// STL
#include <iostream>
#include <vector>
#include <string>
#include <typeindex>
#include <map>

// podio specification
#include "podio/EventStore.h"
#include "podio/HDF5Writer.h"

// HDF5 specific includes
#include "H5Cpp.h"



namespace podio
{

	class CollectionBase;


	class WriterRegister
	{

		public:
			std::map<std::type_index, HDF5Writer*> h5map;
			static WriterRegister* getInstance();
			void register_writer(HDF5Writer* writer);

		private:
			static WriterRegister* instance;
			WriterRegister();
	};

/* Initially the instance is NULL */
WriterRegister* WriterRegister::instance = 0;

}
#endif
