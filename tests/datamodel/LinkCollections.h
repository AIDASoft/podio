#ifndef PODIO_TESTS_LINKCOLLECTIONS_H
#define PODIO_TESTS_LINKCOLLECTIONS_H

#include "podio/LinkCollection.h"

#include "datamodel/ExampleClusterCollection.h"
#include "datamodel/ExampleHitCollection.h"
#include "datamodel/TypeWithEnergy.h"

// Define an link that is used for the I/O tests
using TestLinkCollection = podio::LinkCollection<ExampleHit, ExampleCluster>;
// Define a link with an interace type that is used for I/O tests
using TestInterfaceLinkCollection = podio::LinkCollection<ExampleCluster, TypeWithEnergy>;

#endif // PODIO_TESTS_LINKCOLLECTIONS_H
