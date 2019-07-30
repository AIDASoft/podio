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

// HDF5 specific includes
#include "H5Cpp.h"

using namespace H5;

namespace podio
{

	class CollectionBase;
	class Registry;



class HDF5Writer
{
	public:
		std::map<std::string, std::type_index> Fmap;
		HDF5Writer();
		HDF5Writer(const H5std_string FILE_NAME, EventStore* store);
		template<typename T>void registerForWrite(const std::string& name);
		void writeEvent(std::map<std::type_index, HDF5Writer*> h5map);
		virtual std::type_index get_typeindex();
		virtual void writeCollection(CollectionBase* col, H5File& file);

	private:
		H5std_string m_filename;
		EventStore* m_store;
		std::vector<CollectionBase*> m_storedCollections;
		std::vector<std::string> m_collectionNames;

};


template<typename T> void HDF5Writer::registerForWrite(const std::string& name)
{
	const T* tmp_coll(nullptr);
	m_store->get(name, tmp_coll);
	T* coll = const_cast<T*>(tmp_coll);

	if(coll==nullptr)
		{
			std::cerr<<"no such collection to write, throw exception."<<std::endl;
		}

	m_storedCollections.emplace_back(coll);
	m_collectionNames.emplace_back(name);
	Fmap.insert({name, std::type_index(typeid(T))});
}


}
#endif
