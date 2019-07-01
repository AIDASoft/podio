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
const H5std_string FILE_NAME("bug_dummy.h5");
const H5std_string DATASET_NAME_1("EventInfo_data_1");
const H5std_string DATASET_NAME_2("EventInfo_data_2");
const H5std_string EventInfo_Number("Number");

const int RANK = 1;

int main()
{

	try{
	
		CompType mtype_EventInfo(sizeof(EventInfoData));
mtype_EventInfo.insertMember(EventInfo_Number,HOFFSET(EventInfoData, Number), PredType::NATIVE_INT);

		// create file
		std::shared_ptr<H5File> file(new H5File(FILE_NAME, H5F_ACC_TRUNC));
		
		// Initialize some data that we would like to serialize
		std::cout<<"Start Processing"<<std::endl;
		
		auto store = podio::EventStore();
		auto& info_1 = store.create<EventInfoCollection>("info_1");
		auto& info_2 = store.create<EventInfoCollection>("info_2");
		
		// COLLECTION 1
		auto item_1 = EventInfo();
		item_1.Number(20); 
		
		auto item_2 = EventInfo();
		item_2.Number(21);

		info_1.push_back(item_1);
		info_1.push_back(item_2);

	
		// COLLECTION 2
		auto item_3 = EventInfo();
		item_3.Number(22);
		auto item_4 = EventInfo();
		item_4.Number(23);
		auto item_5 = EventInfo();
		item_5.Number(24);
		auto item_6 = EventInfo();
		item_6.Number(25);
		
		info_2.push_back(item_3);
		info_2.push_back(item_4);
		info_2.push_back(item_5);
		info_2.push_back(item_6);


		// DATASET 1
		const hsize_t SIZE_1 = info_1.size();
		hsize_t dim_1[] = {SIZE_1};
		DataSpace space_1(RANK, dim_1);
		std::shared_ptr<DataSet> dataset1(new DataSet(file->createDataSet(DATASET_NAME_1, mtype_EventInfo, space_1)));
		
		
		// DATASET 2
		const hsize_t SIZE_2 = info_2.size();
		hsize_t dim_2[] = {SIZE_2};
		DataSpace space_2(RANK, dim_2);
		std::shared_ptr<DataSet> dataset2(new DataSet(file->createDataSet(DATASET_NAME_2, mtype_EventInfo, space_2)));
	
		
		// Fill 
		info_1->prepareForWrite();
		void* buffer_1 = info_1->_getBuffer();
		EventInfoData** data_1 = reinterpret_cast<EventInfoData**>(buffer_1);

		info_2->prepareForWrite();
		void* buffer_2 = info_2->_getBuffer();
		EventInfoData** data_2 = reinterpret_cast<EventInfoData**>(buffer_2);

		// Write data to file
		dataset1->write(*data_1, mtype_EventInfo);	
		dataset2->write(*data_2, mtype_EventInfo); 
		
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
