#include "H5_ExampleMCData.h"

H5_ExampleMCData::H5_ExampleMCData()
{
	cout<<"Registering ExampleMCData" << endl;

	h5_datatype = H5::CompType(sizeof(ExampleMCData));
	h5_datatype.insertMember("energy", HOFFSET(ExampleMCData, energy), H5::PredType::NATIVE_DOUBLE);
	h5_datatype.insertMember("PDG", HOFFSET(ExampleMCData, PDG), H5::PredType::NATIVE_INT);
	h5_datatype.insertMember("parents_begin", HOFFSET(ExampleMCData, parents_begin), H5::PredType::NATIVE_UINT);
	h5_datatype.insertMember("parents_end", HOFFSET(ExampleMCData, parents_end), H5::PredType::NATIVE_UINT);
	h5_datatype.insertMember("daughters_begin", HOFFSET(ExampleMCData, daughters_begin), H5::PredType::NATIVE_UINT);
	h5_datatype.insertMember("daughters_end", HOFFSET(ExampleMCData, daughters_end), H5::PredType::NATIVE_UINT);

	hsize_t chunk_dims[2] = {1, 5};
	cparms.setChunk(1, chunk_dims);
	flag = 0;


}


H5_ExampleMCData* H5_ExampleMCData::getInstance()
{
    if (instance == 0)
    {
        instance = new H5_ExampleMCData();
    }

    return instance;
}


std::type_index H5_ExampleMCData::get_typeindex()
{
	std::cout << "returning the typeid of ExampleMCDataCollection" <<std::endl;

	return typeid(ExampleMCCollection);
}

void H5_ExampleMCData::writeCollection(CollectionBase* c, H5File& file)
{

	ExampleMCCollection* mcps = static_cast<ExampleMCCollection*>( c );
	void* buffer_1 = mcps->_getBuffer();

	ExampleMCData** data_1 = reinterpret_cast<ExampleMCData**>(buffer_1);

	if(flag == 0)
	{
		size[0] = mcps->size();
		std::cout << "FOR THE FIRST TIME FLAG = 0" <<std::endl;
		/*
		* Initialize the data space with unlimited dimensions.
		*/
		hsize_t dims[1]  = {mcps->size()};
		hsize_t maxdims[1] = {H5S_UNLIMITED};
		DataSpace mspace1( RANK_EMD, dims, maxdims);

		dataset_ExampleMC = file.createDataSet(EXAMPLE_MC_DATA, h5_datatype, mspace1, cparms);
		DataSpace fspace1 = dataset_ExampleMC.getSpace();
		offset[0] = 0;
		hsize_t dims1[1] = {mcps->size()};            /* data1 dimensions */
		fspace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

		dataset_ExampleMC.extend(size);

		// Write data to file
		dataset_ExampleMC.write(*data_1, h5_datatype, mspace1, fspace1);

		// we have written once so set flag = 1
		flag = 1;

	}

	else
	{
		// extend dataset_ExampleMC
		hsize_t dim2[1] = {mcps->size()};

		offset[0] = size[0];
		size[0] += mcps->size();

		dataset_ExampleMC.extend( size );

		// select hyperslab
		DataSpace fspace2 = dataset_ExampleMC.getSpace();
		fspace2.selectHyperslab(H5S_SELECT_SET, dim2, offset );

		DataSpace mspace2( RANK_EMD, dim2 );

		// Write data to file
		dataset_ExampleMC.write(*data_1, h5_datatype, mspace2, fspace2);

	}

}
