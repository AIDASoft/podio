// Data model
#include "EventInfoData.h"
#include "EventInfoCollection.h"

// STL
#include <iostream>
#include <vector>
#include <map>
#include <typeinfo>

// podio specific includes
#include "podio/EventStore.h"
#include "podio/CollectionBase.h"

// HDF5 specific includes
#include "H5Cpp.h"
#include "H5_EventInfoData.h"
#include "podio/HDF5Writer.h"

using namespace H5;	
using namespace std; 

const H5std_string FILE_NAME("dummy.h5");
const H5std_string FILE_NAME_1("dummy_writer.h5");

map<const type_info*, string> m;

int main()
{

	try{
		// create file
		H5File file(FILE_NAME, H5F_ACC_TRUNC);
		
		// create EventStore
		auto store = podio::EventStore();
		auto writer = podio::HDF5Writer(FILE_NAME_1, &store);
		
		auto& info = store.create<EventInfoCollection>("info");
		
		// for writing EventInfoData	
		// H5_EventInfoData h5eid; 	
		
		auto h5eid = H5_EventInfoData::getInstance();

			
		writer.registerForWrite<EventInfoCollection>("info");

		unsigned nevents = 3;
		
		for(unsigned i=0; i<nevents; ++i) 
		{
			std::cout << "processing event " << i << std::endl;

			// we just write the same stuff nevents' times.
			
			auto item_1 = EventInfo();
			item_1.Number(20); 

			auto item_2 = EventInfo();
			item_2.Number(21);

			auto item_3 = EventInfo();
			item_3.Number(22); 

			auto item_4 = EventInfo();
			item_4.Number(23);

			info.push_back(item_1);
			info.push_back(item_2);
			info.push_back(item_3);
			info.push_back(item_4);
			
			h5eid->writeH5(file, info);
			
			writer.writeEvent();

			store.clearCollections();

		
		}
	
	

	} // end of try block
	
	
	// catch failure caused by the H5File operations
	catch( FileIException error )
	{
		error.printErrorStack();
		return -1;
	}
	
	// catch failure caused by the DataSet operations
	catch( DataSetIException error )
	{
		error.printErrorStack();
		return -1;
	}
	// catch failure caused by the DataSpace operations
	catch( DataSpaceIException error )
	{
		error.printErrorStack();
		return -1;
	}
	catch(...)
	{
		std::cout<<"Something terrible happened!"<<std::endl;
		return -1;
	}
	

	return 0;
}
