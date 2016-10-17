#ifndef ExampleWithNamespaceDATA_H
#define ExampleWithNamespaceDATA_H

#include "NamespaceStruct.h"

namespace ex {
/** @class ExampleWithNamespaceData
 *  Type with namespace and namespaced member
 *  @author: Joschka Lingemann
 */

class ExampleWithNamespaceData {
public:
  ex2::NamespaceStruct data;  ///< a component
};
} // namespace ex

#endif
