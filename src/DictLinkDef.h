#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class podio::GenericParameters+;
#pragma link C++ class podio::GenericParameters::MapType<int>+;
#pragma link C++ class podio::GenericParameters::MapType<float>+;
#pragma link C++ class podio::GenericParameters::MapType<double>+;
#pragma link C++ class podio::GenericParameters::MapType<std::string>+;
#pragma link C++ class podio::GenericParameters+;
#pragma link C++ class std::map<int,podio::GenericParameters>+;
#pragma link C++ class std::vector<std::tuple<int, std::string, bool, unsigned int>>+;
#pragma link C++ class std::vector<std::tuple<int, std::string, bool, unsigned>>+;
#pragma link C++ class std::vector<std::tuple<int, std::string, bool>>+;
#pragma link C++ class std::vector<std::tuple<std::string, std::string>>+;
#pragma link C++ class std::vector<std::tuple<uint32_t, std::string, bool, unsigned int>>+;
#pragma link C++ class std::vector<std::tuple<uint32_t, std::string, bool, unsigned>>+;
#pragma link C++ class std::vector<std::tuple<uint32_t, std::string, bool>>+;
#pragma link C++ class podio::CollectionBase+;
#pragma link C++ class podio::CollectionIDTable+;
#pragma link C++ class podio::version::Version+;
#pragma link C++ class podio::ObjectID+;
#pragma link C++ class vector<podio::ObjectID>+;
#pragma link C++ class podio::UserDataCollection<float>+;
#pragma link C++ class podio::UserDataCollection<double>+;
#pragma link C++ class podio::UserDataCollection<int8_t>+;
#pragma link C++ class podio::UserDataCollection<int16_t>+;
#pragma link C++ class podio::UserDataCollection<int32_t>+;
#pragma link C++ class podio::UserDataCollection<int64_t>+;
#pragma link C++ class podio::UserDataCollection<uint8_t>+;
#pragma link C++ class podio::UserDataCollection<uint16_t>+;
#pragma link C++ class podio::UserDataCollection<uint32_t>+;
#pragma link C++ class podio::UserDataCollection<uint64_t>+;

#endif

