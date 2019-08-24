#include "H5ExampleWithArrayCollection.h"
H5ExampleWithArrayCollection::H5ExampleWithArrayCollection()
{
	hsize_t p_array_dim[] = {4};
	hsize_t myArray_array_dim[] = {4};
	hsize_t anotherArray2_array_dim[] = {4};
	hsize_t snail_case_array_array_dim[] = {4};
	hsize_t snail_case_Array3_array_dim[] = {4};
	hsize_t structArray_array_dim[] = {4};

	CompType mtype_SimpleStruct(sizeof(SimpleStruct));
	mtype_SimpleStruct.insertMember("y", HOFFSET(SimpleStruct, y),PredType::NATIVE_INT);
	mtype_SimpleStruct.insertMember("x", HOFFSET(SimpleStruct, x),PredType::NATIVE_INT);
	mtype_SimpleStruct.insertMember("z", HOFFSET(SimpleStruct, z),PredType::NATIVE_INT);
	mtype_SimpleStruct.insertMember("p", HOFFSET(SimpleStruct, p),H5Tarray_create(H5T_NATIVE_INT, 1, p_array_dim));

	CompType mtype_NotSoSimpleStruct(sizeof(NotSoSimpleStruct));
	mtype_NotSoSimpleStruct.insertMember("data", HOFFSET(NotSoSimpleStruct, data),mtype_SimpleStruct);

	CompType mtype_NamespaceStruct(sizeof(NamespaceStruct));
	mtype_NamespaceStruct.insertMember("y", HOFFSET(NamespaceStruct, y),PredType::NATIVE_INT);
	mtype_NamespaceStruct.insertMember("x", HOFFSET(NamespaceStruct, x),PredType::NATIVE_INT);

	h5_datatype = CompType(sizeof(ExampleWithArrayData));
	h5_datatype.insertMember("arrayStruct", HOFFSET(ExampleWithArrayData, arrayStruct),mtype_NotSoSimpleStruct);
	h5_datatype.insertMember("myArray", HOFFSET(ExampleWithArrayData, myArray),H5Tarray_create(H5T_NATIVE_INT, 1, myArray_array_dim));
	h5_datatype.insertMember("anotherArray2", HOFFSET(ExampleWithArrayData, anotherArray2),H5Tarray_create(H5T_NATIVE_INT, 1, anotherArray2_array_dim));
	h5_datatype.insertMember("snail_case_array", HOFFSET(ExampleWithArrayData, snail_case_array),H5Tarray_create(H5T_NATIVE_INT, 1, snail_case_array_array_dim));
	h5_datatype.insertMember("snail_case_Array3", HOFFSET(ExampleWithArrayData, snail_case_Array3),H5Tarray_create(H5T_NATIVE_INT, 1, snail_case_Array3_array_dim));
	h5_datatype.insertMember("structArray", HOFFSET(ExampleWithArrayData, structArray),H5Tarray_create(mtype_NamespaceStruct.getId(), 1, structArray_array_dim));

	event_count = 0;
}


H5ExampleWithArrayCollection* H5ExampleWithArrayCollection::getInstance()
{
	if (instance == 0)
		{
			instance = new H5ExampleWithArrayCollection();
		}

	return instance;
}



ExampleWithArrayCollection* H5ExampleWithArrayCollection::get_collection(const std::string& name, EventStore* m_store)
{
	const ExampleWithArrayCollection* tmp_coll(nullptr);
	m_store->get<ExampleWithArrayCollection>(name, tmp_coll);
	ExampleWithArrayCollection* coll = const_cast<ExampleWithArrayCollection*>(tmp_coll);
	return coll;
}


void H5ExampleWithArrayCollection::writeCollection(CollectionBase* c, H5File& file)
{
	event_count += 1;

	ExampleWithArrayCollection* info = static_cast<ExampleWithArrayCollection*>( c );
	void* buffer = info->_getBuffer();
	ExampleWithArrayData** data = reinterpret_cast<ExampleWithArrayData**>(buffer);

	const int SIZE = info->size();
	if(SIZE > 0)
	{
		hsize_t dim[] = {static_cast<hsize_t>(SIZE)};
		DataSpace space(RANK_EXAMPLEWITHARRAY, dim);
		string group_name = std::to_string(event_count-1);
		string dataset_name = group_name + "/ExampleWithArray";

		Group g;
		if (pathExists(file.getId(), group_name))
			g=file.openGroup(group_name.c_str());
		else
			g=file.createGroup(group_name.c_str());

		DataSet d = file.createDataSet(dataset_name.c_str(), h5_datatype, space);
		// Write data to file
		d.write(*data, h5_datatype);
	}
}
