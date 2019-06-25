#include "EventInfoCollection.h"
#include <iostream>
#include <vector>

#include "podio/EventStore.h"

// TODO: Need to create a header file for HDF5Writer
//#include <podio/HDF5Writer.h> 

#include "H5Cpp.h"
#include <memory>

using namespace H5;	

int main()
{

	// Strategy I:
	// create struct which holds the info that we want to serialize
	// then use this struct to create hdf5 compound type 


	// Create struct
	typedef struct event_info
	{
		int Number;
		unsigned int id;	
	}event_info;

	// declare file name, dataset name and member names of the compound type
	const H5std_string FILE_NAME("dummy.h5");
	const H5std_string DATASET_NAME("EventInfoCollection");
	const H5std_string MEMBER1("Number");
	const H5std_string MEMBER2("id");
	
	const int RANK = 1;
	
	// create file
	std::shared_ptr<H5File> file(new H5File(FILE_NAME, H5F_ACC_TRUNC));
	
	// declare compound datatype
	CompType mtype(sizeof(event_info));
	mtype.insertMember(MEMBER1, HOFFSET(event_info, Number), PredType::NATIVE_INT);
	mtype.insertMember(MEMBER2, HOFFSET(event_info, id), PredType::NATIVE_INT);	
	
	// create dataset
	const int SIZE = 2; // only putting two objects of type EventInfo in the collection 
	hsize_t dim[] = {SIZE}; 
	DataSpace space(RANK, dim);
	
std::shared_ptr<DataSet> dataset(new DataSet(file->createDataSet(DATASET_NAME, mtype, space)));
	
	std::cout<<"Start Processing"<<std::endl;

	auto store = podio::EventStore();
	auto& info = store.create<EventInfoCollection>("info");
	
	// Declare two EventInfo objects
	// and put them in the struct event_info
	auto item1 = EventInfo();
	item1.Number(20);
	
	auto item2 = EventInfo();
	item2.Number(21);

	// make a collection
	info.push_back(item1);
	info.push_back(item2);
	
	// but... we need an array of structs
	// can't use a vector since write() method accepts const void*
	// no known conversion from std::vector to const void*
	
	event_info* p = (event_info*) malloc(SIZE * sizeof(event_info));

	for(size_t i=0; i<info.size(); i++)
		{
			std::cout<<"Adding Event"<<std::endl;
			std::cout<< info[i]<<std::endl;
			
			p[i].Number = info[i].Number();
			p[i].id = info[i].id();
		}

	// finally put this in the file
	dataset->write(p, mtype);

	store.clearCollections();
}

