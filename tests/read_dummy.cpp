// Data model
#include "EventInfoData.h"
#include "EventInfoCollection.h"

// STL
#include <iostream>
#include <vector>
#include <memory>

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
		std::shared_ptr<H5File> file(new H5File(FILE_NAME, H5F_ACC_RDONLY));
	
		// DATASET 1
		std::shared_ptr<DataSet> dataset1(new DataSet(file->openDataSet(DATASET_NAME_1)));
		
		// DATASET 2
		std::shared_ptr<DataSet> dataset2(new DataSet(file->openDataSet(DATASET_NAME_2)));
	
		// Buffer to store data for Dataset 1 
		EventInfoData data_1[2]; 
		
		EventInfoData data_2[4];

		// Read data to file
		std::cout<<"Reading data..."<<std::endl;

		dataset1->read(data_1, mtype_EventInfo);	
		dataset2->read(data_2, mtype_EventInfo);
		
		// print data to verify
		std::cout<<"Dataset I"<<std::endl;
		
		for(int i=0; i<2; i++)	
			std::cout<<data_1[i].Number<<std::endl; 
			
		std::cout<<"Dataset II"<<std::endl;

		for(int i=0; i<4; i++)	
			std::cout<<data_2[i].Number<<std::endl;
		

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
