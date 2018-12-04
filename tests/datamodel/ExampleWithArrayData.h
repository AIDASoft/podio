#ifndef ExampleWithArrayDATA_H
#define ExampleWithArrayDATA_H

#include "NamespaceStruct.h"
#include <array>

#include "NotSoSimpleStruct.h"

/** @class ExampleWithArrayData
 *  Datatype with an array member
 *  @author: Joschka Lingemann
 */

class ExampleWithArrayData {
public:
  NotSoSimpleStruct arrayStruct; ///< component that contains an array
  std::array<int, 4> myArray;    ///< array-member without space to test regex
  std::array<int, 4> anotherArray2; ///< array-member with space to test regex
  std::array<int, 4> snail_case_array;  ///< snail case to test regex
  std::array<int, 4> snail_case_Array3; ///< mixing things up for regex
  std::array<ex2::NamespaceStruct, 4>
      structArray; ///< an array containing structs
};

#endif
