#ifndef PODIO_TESTS_FRAME_TEST_COMMON_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_FRAME_TEST_COMMON_H // NOLINT(llvm-header-guard): folder structure not suitable

#include <string>
#include <vector>

static const std::vector<std::string> collsToWrite = {"mcparticles",
                                                      "moreMCs",
                                                      "arrays",
                                                      "mcParticleRefs",
                                                      "hits",
                                                      "hitRefs",
                                                      "refs",
                                                      "refs2",
                                                      "clusters",
                                                      "OneRelation",
                                                      "info",
                                                      "WithVectorMember",
                                                      "fixedWidthInts",
                                                      "userInts",
                                                      "userDoubles",
                                                      "WithNamespaceMember",
                                                      "WithNamespaceRelation",
                                                      "WithNamespaceRelationCopy",
                                                      "emptyCollection",
                                                      "emptySubsetColl",
                                                      "associations"};

#endif // PODIO_TESTS_FRAME_TEST_COMMON_H
