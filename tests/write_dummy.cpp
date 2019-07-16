// Data model
#include "EventInfoData.h"
#include "EventInfoCollection.h"
#include "ExampleMCData.h"
#include "ExampleMCCollection.h"

// STL
#include <iostream>
#include <vector>
#include <map>

// podio specific includes
#include "podio/EventStore.h"
#include "podio/CollectionBase.h"

// HDF5 specific includes
#include "H5Cpp.h"
#include "H5_EventInfoData.h"
#include "H5_ExampleMCData.h"


using namespace H5;	
using namespace std; 

const H5std_string FILE_NAME("dummy.h5");
const H5std_string EVENT_INFO_DATA("EventInfoData");

const int RANK = 1;

int main()
{

	try{
		// create file
		H5File file(FILE_NAME, H5F_ACC_TRUNC);
		
		// create EventStore
		auto store = podio::EventStore();
		auto& info = store.create<EventInfoCollection>("info");
		
		// CompType + cparms + size + offset for EventInfoData
		H5_EventInfoData h5eid; 
				
		/*
		* Create a new dataset_eventInfo within the file using cparms
		* creation properties.
		*/
		DataSet dataset_eventInfo; 
	
		unsigned nevents = 3;
		
		for(unsigned i=0; i<nevents; ++i) 
		{
			std::cout << "processing event " << i << std::endl;

			// COLLECTION 1
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

			// Fill 
			info->prepareForWrite();
			void* buffer_1 = info->_getBuffer();
			EventInfoData** data_1 = reinterpret_cast<EventInfoData**>(buffer_1);
			
			
			
			if(i==0)
			{		
				h5eid.size[0] = info.size();
				
				/*
				* Initialize the data space with unlimited dimensions.
				*/
				hsize_t dims[1]  = {info.size()};  
				hsize_t maxdims[1] = {H5S_UNLIMITED};
				DataSpace mspace1( RANK, dims, maxdims);
				
				dataset_eventInfo = file.createDataSet(EVENT_INFO_DATA, h5eid.h5dt(), mspace1, h5eid.cp());
						
				DataSpace fspace1 = dataset_eventInfo.getSpace();
				h5eid.offset[0] = 0;
				hsize_t dims1[1] = {info.size()};            /* data1 dimensions */
				fspace1.selectHyperslab( H5S_SELECT_SET, dims1, h5eid.offset );
				
				dataset_eventInfo.extend(h5eid.size);
				
				// Write data to file
				dataset_eventInfo.write(*data_1, h5eid.h5dt(), mspace1, fspace1);	
						
							

			}
			
			
			else
			{	
				// extend dataset_eventInfo
				hsize_t dim2[1] = {info.size()};
				
				h5eid.offset[0] = h5eid.size[0];
				h5eid.size[0] += info.size();

				dataset_eventInfo.extend( h5eid.size );

				// select hyperslab
				DataSpace fspace2 = dataset_eventInfo.getSpace();
				fspace2.selectHyperslab(H5S_SELECT_SET, dim2, h5eid.offset );

				DataSpace mspace2( RANK, dim2 );	


				// Write data to file
				dataset_eventInfo.write(*data_1, h5eid.h5dt(), mspace2, fspace2 );	
			}
				
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
