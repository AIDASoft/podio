#ifndef HDF5_WRITER_H
#define HDF5_WRITER_H

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
#include "podio/H5_Writer_Collections.h"

// HDF5 specific includes
#include "H5Cpp.h"

using namespace H5;

namespace podio
{

	class CollectionBase;

class HDF5Writer
{
	public:
		std::map<std::string, H5_Writer_Collections*> Fmap;
		HDF5Writer(const H5std_string FILE_NAME, EventStore* store);
		template<typename T>void registerForWrite(const std::string& name);
		void writeEvent();


	private:
		H5std_string m_filename;
		EventStore* m_store;
		std::vector<CollectionBase*> m_storedCollections;
		std::vector<std::string> m_collectionNames;

};


template<typename T> void HDF5Writer::registerForWrite(const std::string& name)
{

	T* tmp_writer = T::getInstance();
	auto coll = tmp_writer->get_collection(name, m_store);

	if(coll==nullptr)
		{
			std::cerr<<"no such collection to write, throw exception."<<std::endl;
		}

	m_storedCollections.emplace_back(coll);
	m_collectionNames.emplace_back(name);
	Fmap.insert({name, tmp_writer});
}


}
#endif
