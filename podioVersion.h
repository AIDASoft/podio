#ifndef podioVersion_h
#define podioVersion_h
#define podio_VERSION @podio_VERSION@
#define podio_VERSION_MAJOR @podio_VERSION_MAJOR@
#define podio_VERSION_MINOR @podio_VERSION_MINOR@
#define podio_VERSION_PATCH @podio_VERSION_PATCH@
#define podio_VERSION_ENCODE(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define podio_VERSION_CODE podio_VERSION_ENCODE(podio_VERSION_MAJOR,podio_VERSION_MINOR,podio_VERSION_PATCH)
#endif
