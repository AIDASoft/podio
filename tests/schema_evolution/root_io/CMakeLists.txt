PODIO_CREATE_WRITE_OLD_DATA_TEST(write_old_data_root.cpp "TestDataModel_v1Dict;podio::podioRootIO")

CREATE_PODIO_TEST(read_new_data_root.cpp "TestDataModelDict;podioRootIO")
set_property(TEST read_new_data_root PROPERTY DEPENDS write_old_data_root)
