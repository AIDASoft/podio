// Data model
#include "H5EventInfoCollection.h"

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

int main()
{

	try{


		// open file in read only mode
		H5File file(FILE_NAME, H5F_ACC_RDONLY);

        //auto reader = podio::ROOTReader();
        auto store = podio::EventStore();
        //reader.openFile("example.root");
        //store.setReader(&reader);

        //unsigned nEvents = reader.getEntries();
        unsigned nEvents = 2;
        auto eid = H5EventInfoCollection::getInstance();
     

        for(unsigned i=0; i<nEvents; ++i)
        {
            eid->readCollection(i, file, store);
            
        }

        return 0;

	} // end of try block

	// catch failure caused by the H5File operations
	catch( FileIException error )
	{
		error.printError();
		return -1;
	}
	// catch failure caused by the DataSet operations
	catch( DataSetIException error )
	{
		error.printError();
		return -1;
	}
	// catch failure caused by the DataSpace operations
	catch( DataSpaceIException error )
	{
		error.printError();
		return -1;
	}
	catch(...)
	{
		std::cout<<"Something terrible happened!"<<std::endl;
		return -1;
	}
	return 0;
}
