#include <iostream>
#include "H5Cpp.h"
#include "EventInfoData.h"

#ifndef H5_EVENTINFODATA_H
#define H5_EVENTINFODATA_H

using namespace H5;

class H5_EventInfoData
{
	private:
		CompType h5_datatype;
		DSetCreatPropList cparms;
		
	public:
		hsize_t size[1];
		hsize_t offset[1];

	public:
		H5_EventInfoData();

		auto h5dt()
			{
				return h5_datatype;
			}
		auto cp()
			{
				return cparms;
			}

};

H5_EventInfoData::H5_EventInfoData() 
{
	
	h5_datatype = CompType(sizeof(EventInfoData));
	h5_datatype.insertMember("Number", HOFFSET(EventInfoData, Number), H5::PredType::NATIVE_INT);

	hsize_t chunk_dims[2] = {1, 5};
	cparms.setChunk(1, chunk_dims);


}


#endif
