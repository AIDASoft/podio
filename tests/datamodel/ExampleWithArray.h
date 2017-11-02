#ifndef ExampleWithArray_H
#define ExampleWithArray_H
#include <array>
#include "NamespaceStruct.h"

#include "NotSoSimpleStruct.h"
#include "ExampleWithArrayData.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include "podio/ObjectID.h"

//forward declarations


#include "ExampleWithArrayConst.h"
#include "ExampleWithArrayObj.h"



class ExampleWithArrayCollection;
class ExampleWithArrayCollectionIterator;
class ConstExampleWithArray;

/** @class ExampleWithArray
 *  Datatype with an array member
 *  @author: Joschka Lingemann
 */
class ExampleWithArray {

  friend ExampleWithArrayCollection;
  friend ExampleWithArrayCollectionIterator;
  friend ConstExampleWithArray;

public:

  /// default constructor
  ExampleWithArray();
  ExampleWithArray(NotSoSimpleStruct arrayStruct,std::array<int, 4> myArray,std::array<int, 4> anotherArray2,std::array<int, 4> snail_case_array,std::array<int, 4> snail_case_Array3,std::array<ex2::NamespaceStruct, 4> structArray);

  /// constructor from existing ExampleWithArrayObj
  ExampleWithArray(ExampleWithArrayObj* obj);
  /// copy constructor
  ExampleWithArray(const ExampleWithArray& other);
  /// copy-assignment operator
  ExampleWithArray& operator=(const ExampleWithArray& other);
  /// support cloning (deep-copy)
  ExampleWithArray clone() const;
  /// destructor
  ~ExampleWithArray();

  /// conversion to const object
  operator ConstExampleWithArray () const;

public:

  /// Access the  component that contains an array
  const NotSoSimpleStruct& arrayStruct() const;
  /// Access the member of  component that contains an array
  const SimpleStruct& data() const;
  /// Access the array-member without space to test regex
  const std::array<int, 4>& myArray() const;
  /// Access item i in the array-member without space to test regex
  const int& myArray(size_t i) const;
  /// Access the array-member with space to test regex
  const std::array<int, 4>& anotherArray2() const;
  /// Access item i in the array-member with space to test regex
  const int& anotherArray2(size_t i) const;
  /// Access the snail case to test regex
  const std::array<int, 4>& snail_case_array() const;
  /// Access item i in the snail case to test regex
  const int& snail_case_array(size_t i) const;
  /// Access the mixing things up for regex
  const std::array<int, 4>& snail_case_Array3() const;
  /// Access item i in the mixing things up for regex
  const int& snail_case_Array3(size_t i) const;
  /// Access the an array containing structs
  const std::array<ex2::NamespaceStruct, 4>& structArray() const;
  /// Access item i in the an array containing structs
  const ex2::NamespaceStruct& structArray(size_t i) const;

  /// Get reference to the  component that contains an array
  NotSoSimpleStruct& arrayStruct();
  /// Set the  component that contains an array
  void arrayStruct(class NotSoSimpleStruct value);
  /// Get reference to the member of  component that contains an array
  SimpleStruct& data();
  /// Set the  member of  component that contains an array
  void data(class SimpleStruct value);
  /// Set the array-member without space to test regex
  void myArray(std::array<int, 4> value);

  /// Set item i in array-member without space to test regex
  void myArray(size_t i, int value);

  /// Set the array-member with space to test regex
  void anotherArray2(std::array<int, 4> value);

  /// Set item i in array-member with space to test regex
  void anotherArray2(size_t i, int value);

  /// Set the snail case to test regex
  void snail_case_array(std::array<int, 4> value);

  /// Set item i in snail case to test regex
  void snail_case_array(size_t i, int value);

  /// Set the mixing things up for regex
  void snail_case_Array3(std::array<int, 4> value);

  /// Set item i in mixing things up for regex
  void snail_case_Array3(size_t i, int value);

  /// Set the an array containing structs
  void structArray(std::array<ex2::NamespaceStruct, 4> value);

  /// Set item i in an array containing structs
  void structArray(size_t i, ex2::NamespaceStruct value);




  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithArrayObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleWithArray& other) const {
    return (m_obj==other.m_obj);
  }

  bool operator==(const ConstExampleWithArray& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithArray& p1,
//       const ExampleWithArray& p2 );
  bool operator<(const ExampleWithArray& other) const { return m_obj < other.m_obj  ; }


  unsigned int id() const { return getObjectID().collectionID * 10000000 + getObjectID().index  ;  } 

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithArrayObj* m_obj;

};

std::ostream& operator<<( std::ostream& o,const ConstExampleWithArray& value );




#endif
