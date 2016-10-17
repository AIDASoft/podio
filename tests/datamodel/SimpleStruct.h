#ifndef SimpleStruct_H
#define SimpleStruct_H


#include <iostream>


class SimpleStruct {
public:
  int x;
  int y;
  int z;

 SimpleStruct() : x(0),y(0),z(0) {} SimpleStruct( const int* v) : x(v[0]),y(v[1]),z(v[2]) {} 
};





#endif
