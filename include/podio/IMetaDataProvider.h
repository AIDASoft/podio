#ifndef PODIO_IMETADATAPROVIDER_H
#define PODIO_IMETADATAPROVIDER_H

#include "podio/GenericParameters.h"
#include "podio/utilities/Deprecated.h"

namespace podio {

/** Inteface to access meta data for runs, events and collections.
 * @author F. Gaede, DESY
 * @date Apr 2020
 */
class DEPR_EVTSTORE IMetaDataProvider {

public:
  /// destructor
  virtual ~IMetaDataProvider() = default;

  /// return the event meta data for the current event
  virtual GenericParameters& getEventMetaData() = 0;

  /// return the run meta data for the given runID
  virtual GenericParameters& getRunMetaData(int runID) = 0;

  /// return the collection meta data for the given colID
  virtual GenericParameters& getCollectionMetaData(uint32_t colID) = 0;
};

} // namespace podio

#endif
