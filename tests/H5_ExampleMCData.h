#include <iostream>
#include "H5Cpp.h"
#include "ExampleMCData.h"

#ifndef H5_EXAMPLEMCDATA_H
#define H5_EXAMPLEMCDATA_H

class H5_ExampleMCData
{
	private:
		H5::CompType h5_datatype;
	
	public:
		H5_ExampleMCData();

		auto h5dt()
			{
				return h5_datatype;
			}
};


H5_ExampleMCData::H5_ExampleMCData() 
{
	
	h5_datatype = H5::CompType(sizeof(ExampleMCData));
	h5_datatype.insertMember("energy", HOFFSET(ExampleMCData, energy), H5::PredType::NATIVE_DOUBLE);
	h5_datatype.insertMember("PDG", HOFFSET(ExampleMCData, PDG), H5::PredType::NATIVE_INT);
	h5_datatype.insertMember("parents_begin", HOFFSET(ExampleMCData, parents_begin), H5::PredType::NATIVE_UINT);
	h5_datatype.insertMember("parents_end", HOFFSET(ExampleMCData, parents_end), H5::PredType::NATIVE_UINT);
	h5_datatype.insertMember("daughters_begin", HOFFSET(ExampleMCData, daughters_begin), H5::PredType::NATIVE_UINT);
	h5_datatype.insertMember("daughters_end", HOFFSET(ExampleMCData, daughters_end), H5::PredType::NATIVE_UINT);
	
	
}

#endif
