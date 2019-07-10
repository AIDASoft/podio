#include <iostream>
#include "H5Cpp.h"
#include "EventInfoData.h"

#ifndef H5_EVENTINFODATA_H
#define H5_EVENTINFODATA_H

class H5_EventInfoData
{
	private:
		H5::CompType h5_datatype;
	
	public:
		H5_EventInfoData();

		auto h5dt()
			{
				return h5_datatype;
			}
};


H5_EventInfoData::H5_EventInfoData() 
{
	
	h5_datatype = H5::CompType(sizeof(EventInfoData));
	h5_datatype.insertMember("Number", HOFFSET(EventInfoData, Number), H5::PredType::NATIVE_INT);
}

#endif
