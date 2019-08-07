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
#include "H5_ExampleMCData.h"


using namespace H5;
using namespace std;
using namespace podio;

const H5std_string FILE_NAME("dummy.h5");

int main()
{

	try{

		// create EventStore
		auto store = EventStore();

		// declare HDF5Writer
		auto writer = HDF5Writer(FILE_NAME, &store);

		// declare register
		auto wr = podio::WriterRegister::getInstance();
		// we should register EventInfoData writer
		wr->register_writer(H5_EventInfoData::getInstance());
		wr->register_writer(H5_ExampleMCData::getInstance());
			
		// create EventInfoCollection
		auto& info = store.create<EventInfoCollection>("info");
		auto& mcps = store.create<ExampleMCCollection>("mcparticles");
		
		writer.registerForWrite<EventInfoCollection>("info");
		writer.registerForWrite<ExampleMCCollection>("mcparticles");
		

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
			
			// ---- add some MC particles ----
			auto mcp0 = ExampleMC();
			auto mcp1 = ExampleMC();
			auto mcp2 = ExampleMC();

			mcps.push_back( mcp0 ) ;
			mcps.push_back( mcp1 ) ;
			mcps.push_back( mcp2 ) ;

			// --- add some daughter relations
			auto p = ExampleMC();
			auto d = ExampleMC();
			
			p = mcps[0] ; 
			p.adddaughters( mcps[1] ) ;
			p.adddaughters( mcps[2] ) ;

			//--- now fix the parent relations
			for( unsigned j=0,N=mcps.size();j<N;++j)
			{
				p = mcps[j] ; 
				for(auto it = p.daughters_begin(), end = p.daughters_end() ; it!=end ; ++it )
				{
					int dIndex = it->getObjectID().index ;
					d = mcps[ dIndex ] ;
					d.addparents( p ) ;
				}
			}

    
			writer.writeEvent(wr->h5map);
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
