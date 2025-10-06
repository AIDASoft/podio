#include "datamodel/NamespaceTypeCollection.h"
#include "datamodel/TestTypeCollection.h"

#include "check_base.h"

int main() {
  WRITE_WITH(WriterT, (TEST_CASE FILE_SUFFIX), {
    auto coll = TestTypeCollection();
    auto elem = coll.create();
    elem.comp().x = 123;

    auto coll2 = nsp::NamespaceTypeCollection();
    auto elem2 = coll2.create();
    elem2.comp().f = 3.14f;
    elem2.singleRel(elem2);
    elem2.addmultiRel(elem);

    event.put(std::move(coll), "Collection1");
    event.put(std::move(coll2), "Collection2");
  });

  READ_WITH(ReaderT, (TEST_CASE FILE_SUFFIX), {
    const auto& coll = event.get<TestTypeCollection>("Collection1");
    const auto elem = coll[0];
    ASSERT_EQUAL(elem.comp().x, 123, "Data changed although schema is unchanged");

    const auto& coll2 = event.get<nsp::NamespaceTypeCollection>("Collection2");
    const auto elem2 = coll2[0];

    ASSERT_EQUAL(elem2.comp().f, 3.14f, "Data changed although schema is unchanged");
    ASSERT_EQUAL(elem2.singleRel(), elem2, "Relation changed although schema is unchanged");
    ASSERT_EQUAL(elem2.multiRel()[0], elem, "Relation changed although schema is unchanged");
  });
}
