#ifndef HDF5_WRITER_H
#define HDF5_WRITER_H

// STL
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <map>

// podio specification
#include "podio/EventStore.h"

// HDF5 specific includes
#include "H5Cpp.h" 

namespace podio
{

	class CollectionBase;
	class Registry;

	struct _ColWriterBase 
	{
		virtual void writeCollection(CollectionBase* c)=0;
	};

template<class T> struct _ColWriter:public _ColWriterBase
{
	void writeCollection(CollectionBase* c)
		{
			
			// We know that we are writing EventInfoData
			T* col = static_cast<T*>( c );			
			std::cout<<col->size()<<std::endl;
			std::cout<<*col<<std::endl;
		}
};

typedef std::map<std::string, _ColWriterBase* > _FunMap;

class HDF5Writer
{
	public:
		
		HDF5Writer(const H5std_string FILE_NAME, EventStore* store);
		~HDF5Writer();
		template<typename T>void registerForWrite(const std::string& name);
		void writeEvent();
		void finish();

	private:
		template <typename T> void writeCollection(const std::string& name);
		H5std_string m_filename;
		EventStore* m_store;
		std::vector<CollectionBase*> m_storedCollections;
		std::vector<std::string> m_collectionNames;
		_FunMap m_map;

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
	m_map[ name ] = new _ColWriter<T>;
}

}
#endif
