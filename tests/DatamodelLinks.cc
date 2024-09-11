#include "podio/LinkCollection.h"

#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/TypeWithEnergy.h"

PODIO_DECLARE_LINK(ExampleHit, ExampleCluster)
PODIO_DECLARE_LINK(ExampleCluster, TypeWithEnergy)
