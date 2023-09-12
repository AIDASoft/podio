CREATE_PODIO_TEST(write_old_data_root.cpp "TestDataModelDict;podioRootIO")
PODIO_CREATE_READ_NEW_DATA_TEST(read_new_data_root.cpp "TestDataModel_v3Dict;podio::podioRootIO")

set_property(TEST read_new_data_root PROPERTY DEPENDS write_old_data_root)
