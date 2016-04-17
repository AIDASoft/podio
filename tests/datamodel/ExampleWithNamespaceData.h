#ifndef ExampleWithNamespaceDATA_H
#define ExampleWithNamespaceDATA_H

// Type with namespace and namespaced member
// author: Joschka Lingemann

#include "NamespaceStruct.h"

namespace ex {
class ExampleWithNamespaceData {
public:
  ex2::NamespaceStruct data;  ///< a component
};
} // namespace ex

#endif
