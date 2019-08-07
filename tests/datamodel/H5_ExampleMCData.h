#ifndef H5_EXAMPLEMCDATA_H
#define H5_EXAMPLEMCDATA_H

#include <iostream>
#include <typeindex>
#include "H5Cpp.h"
#include "ExampleMCData.h"
#include "ExampleMCCollection.h"


#include "podio/EventStore.h"
#include "podio/CollectionBase.h"
#include "podio/HDF5Writer.h"



using namespace H5;
using namespace std;
using namespace podio;

const H5std_string EXAMPLE_MC_DATA("ExampleMCData");
const int RANK_EMD = 1;

class H5_ExampleMCData : public podio::HDF5Writer
{
	public:
		CompType h5_datatype;
		DSetCreatPropList cparms;
		DataSet dataset_ExampleMC;
		hsize_t size[1];
		hsize_t offset[1];
		int flag; // if flag = 0 then we are writing the dataset for the first time

	public:
		void writeCollection(CollectionBase* c, H5File& file);
		static H5_ExampleMCData* getInstance();
		std::type_index get_typeindex();

	private:
		static H5_ExampleMCData* instance;
		H5_ExampleMCData();
};

/* Initially the instance is NULL */
H5_ExampleMCData* H5_ExampleMCData::instance = 0;


#endif
