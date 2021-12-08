#ifndef IMETADATAPROVIDER_H
#define IMETADATAPROVIDER_H

#include "podio/GenericParameters.h"

namespace podio {


   /** Inteface to access meta data for runs, events and collections.
   * @author F. Gaede, DESY
   * @date Apr 2020
   */
  class IMetaDataProvider {

  public:
    /// destructor
    virtual ~IMetaDataProvider(){};

    /// return the event meta data for the current event
    virtual GenericParameters& getEventMetaData() = 0;

    /// return the run meta data for the given runID
    virtual GenericParameters& getRunMetaData(int runID) = 0;

    /// return the collection meta data for the given colID
    virtual GenericParameters& getCollectionMetaData(int colID) = 0;
  };

} // namespace

#endif
