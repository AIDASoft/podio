#ifndef ConstExampleWithString_H
#define ConstExampleWithString_H
#include <string>
#include "ExampleWithStringData.h"
#include <vector>
#include "podio/ObjectID.h"

// Type with a string
// author: Benedikt Hegner

//forward declarations


#include "ExampleWithStringObj.h"



class ExampleWithStringObj;
class ExampleWithString;
class ExampleWithStringCollection;
class ExampleWithStringCollectionIterator;

class ConstExampleWithString {

  friend ExampleWithString;
  friend ExampleWithStringCollection;
  friend ExampleWithStringCollectionIterator;

public:

  /// default constructor
  ConstExampleWithString();
  ConstExampleWithString(std::string theString);

  /// constructor from existing ExampleWithStringObj
  ConstExampleWithString(ExampleWithStringObj* obj);
  /// copy constructor
  ConstExampleWithString(const ConstExampleWithString& other);
  /// copy-assignment operator
  ConstExampleWithString& operator=(const ConstExampleWithString& other);
  /// support cloning (deep-copy)
  ConstExampleWithString clone() const;
  /// destructor
  ~ConstExampleWithString();


public:

  const std::string& theString() const;



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithStringObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ConstExampleWithString& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ExampleWithString& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithString& p1,
//       const ExampleWithString& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithStringObj* m_obj;

};


#endif
