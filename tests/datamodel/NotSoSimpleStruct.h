#ifndef NotSoSimpleStruct_H
#define NotSoSimpleStruct_H
#include "SimpleStruct.h"


#include <iostream>


class NotSoSimpleStruct {
public:
  SimpleStruct data;


};

inline std::ostream& operator<<( std::ostream& o,const NotSoSimpleStruct& value ){ 
  o << value.data << " " ;
  return o ;
}




#endif
