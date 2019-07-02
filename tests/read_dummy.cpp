// Data model
#include "EventInfoData.h"
#include "EventInfoCollection.h"

// STL
#include <iostream>
#include <vector>
#include <cstring>
// podio specific includes
#include "podio/EventStore.h"


// HDF5 specific includes
#include "H5Cpp.h"

using namespace H5;
const H5std_string FILE_NAME( "dummy.h5" );
const H5std_string DATASET_NAME_1( "EventInfo_data_1" );
const H5std_string DATASET_NAME_2( "EventInfo_data_2" );
const H5std_string EventInfo_Number( "Number" );
const int RANK = 1;




int main()
{

	try{
	
		CompType mtype_EventInfo(sizeof(EventInfoData));
		mtype_EventInfo.insertMember(EventInfo_Number,HOFFSET(EventInfoData, Number), PredType::NATIVE_INT);

		// open file in read only mode
		H5File file(FILE_NAME, H5F_ACC_RDONLY);
	
		// DATASET 1
		DataSet dataset1 = file.openDataSet(DATASET_NAME_1);
		
		// DATASET 2
		DataSet dataset2 = file.openDataSet(DATASET_NAME_2);
	
		// Extract information from the datasets

		DataSpace dataspace1 = dataset1.getSpace();
		DataSpace dataspace2 = dataset2.getSpace();

		hsize_t dim_1[2];
		hsize_t dim_2[2];

		dataspace1.getSimpleExtentDims(dim_1, NULL);
		dataspace2.getSimpleExtentDims(dim_2, NULL);

		std::cout<<"dimension of dataset 1 = "<< (unsigned long)dim_1[0]<<std::endl;
	        std::cout<<"dimension of dataset 2 = "<<(unsigned long)dim_2[0]<<std::endl;
		
		hsize_t size_1 = dataset1.getStorageSize();
		hsize_t size_2 = dataset2.getStorageSize();

		std::cout<<"storage space for dataset 1"<<(unsigned long)size_1<<std::endl;
		std::cout<<"storage space for dataset 2"<<(unsigned long)size_2<<std::endl; 
		
		// Read data to file
		std::cout<<"Reading data..."<<std::endl;
		
		auto store = podio::EventStore();
		auto& info_1 = store.create<EventInfoCollection>("info_1");
		
		void* buffer_1 = malloc(size_1);	
		dataset1.read(buffer_1, mtype_EventInfo);	
		
		//check if we managed to read the stuff into buffer_1
		EventInfoData* p = reinterpret_cast<EventInfoData*>(buffer_1);
		std::cout<<p[0].Number<<std::endl;
		std::cout<<p[1].Number<<std::endl;
				

			
		void* dest = info_1->_getBuffer();

		memcpy(dest, buffer_1,size_1);

		// SEG FAULT OCCURS HERE when creating EventInfoObj
		info_1->prepareAfterRead();  
		
		// print data to verify
		std::cout<<"Dataset I"<<std::endl;
		std::cout<<info_1<<std::endl;

		store.clearCollections();		

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
