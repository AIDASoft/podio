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
#include "podio/WriterRegister.h"
#include "podio/HDF5Writer.h"

// HDF5 specific includes
#include "H5Cpp.h"
#include "H5_EventInfoData.h"


using namespace H5;
using namespace std;
using namespace podio;

const H5std_string FILE_NAME("dummy.h5");
const H5std_string FILE_NAME_1("dummy_new.h5");

map<const type_info*, string> m;

int main()
{

	try{

		// create EventStore
		auto store = EventStore();

		// declare HDF5Writer
		auto writer = HDF5Writer(FILE_NAME_1, &store);

		// declare register
		WriterRegister* writer_register = WriterRegister::getInstance();
		// we should register EventInfoData writer
		H5_EventInfoData* eid = H5_EventInfoData::getInstance();
		writer_register->register_writer(eid);

		auto& info = store.create<EventInfoCollection>("info");
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

			writer.writeEvent(writer_register->h5map);
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
