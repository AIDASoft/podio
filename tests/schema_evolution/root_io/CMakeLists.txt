PODIO_CREATE_WRITE_OLD_DATA_TEST(write_old_data_root.cpp "TestDataModel;podio::podioRootIO")

CREATE_PODIO_TEST(read_new_data_root.cpp "TestDataModel;podio::podioRootIO")
set_property(TEST read_new_data_root PROPERTY DEPENDS write_old_data_root)
