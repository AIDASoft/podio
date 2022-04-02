#include "podio/SIOBlockUserData.h"

//#define PODIO_ADD_USER_TYPE_SIO(type) static UserDataSIOBlock<type>    _default##type##CollcetionSIOBlock ;

namespace podio {

static SIOBlockUserData<float> _defaultfloatCollcetionSIOBlock;
static SIOBlockUserData<double> _defaultdoubleCollcetionSIOBlock;

static SIOBlockUserData<int8_t> _defaultint8_tCollcetionSIOBlock;
static SIOBlockUserData<int16_t> _defaultint16_tCollcetionSIOBlock;
static SIOBlockUserData<int32_t> _defaultint32_tCollcetionSIOBlock;
static SIOBlockUserData<int64_t> _defaultint64_tCollcetionSIOBlock;

static SIOBlockUserData<uint8_t> _defaultuint8_tCollcetionSIOBlock;
static SIOBlockUserData<uint16_t> _defaultuint16_tCollcetionSIOBlock;
static SIOBlockUserData<uint32_t> _defaultuint32_tCollcetionSIOBlock;
static SIOBlockUserData<uint64_t> _defaultuint64_tCollcetionSIOBlock;

} // namespace podio

// g++ -E ../src/SIOBlockUserData.cc
// PODIO_ADD_USER_TYPE_SIO(int)
// PODIO_ADD_USER_TYPE_SIO(long)
// PODIO_ADD_USER_TYPE_SIO(float)
// PODIO_ADD_USER_TYPE_SIO(double)
// PODIO_ADD_USER_TYPE_SIO(unsigned)
// PODIO_ADD_USER_TYPE_SIO(unsigned int)
// PODIO_ADD_USER_TYPE_SIO(unsigned long)
// PODIO_ADD_USER_TYPE_SIO(char)
// PODIO_ADD_USER_TYPE_SIO(short)
// PODIO_ADD_USER_TYPE_SIO(long long)
// PODIO_ADD_USER_TYPE_SIO(unsigned long long)
// PODIO_ADD_USER_TYPE_SIO(int16_t)
// PODIO_ADD_USER_TYPE_SIO(int32_t)
// PODIO_ADD_USER_TYPE_SIO(int64_t)
// PODIO_ADD_USER_TYPE_SIO(uint16_t)
// PODIO_ADD_USER_TYPE_SIO(uint32_t)
// PODIO_ADD_USER_TYPE_SIO(uint64_t)
