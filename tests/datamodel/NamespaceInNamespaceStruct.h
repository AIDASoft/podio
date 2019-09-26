#ifndef NamespaceInNamespaceStruct_H
#define NamespaceInNamespaceStruct_H
#include "NamespaceStruct.h"


#include <iostream>

namespace ex2 {
class NamespaceInNamespaceStruct {
public:
 ::ex2::NamespaceStruct data;


};

inline std::ostream& operator<<( std::ostream& o,const ex2::NamespaceInNamespaceStruct& value ){ 
  o << value.data << " " ;
  return o ;
}


} // namespace ex2

#endif
