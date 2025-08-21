#include "datamodel/ArrayTestTypeCollection.h"

#include "check_base.h"
#include "datamodel/SimpleStruct.h"

int main() {
  WRITE_AS(ArrayTestTypeCollection, {
    std::array<SimpleStruct, 5> arr = {{{12, 13, 14, {1, 2, 3, 4}},
                                        {22, 23, 24, {11, 22, 33, 44}},
                                        {32, 33, 34, {111, 222, 333, 444}},
                                        {42, 43, 44, {1111, 2222, 3333, 4444}},
                                        {52, 53, 54, {11111, 22222, 33333, 44444}}}};
    elem.s(arr);
  });
  READ_AS(ArrayTestTypeCollection, {
    const auto& arr = elem.s();
    for (const auto& arrElem : arr) {
      ASSERT_EQUAL(arrElem.t, 0, "New member of array component is not zero initialized");
    }
    // Only do a few spot checks here
    ASSERT_EQUAL(arr[0].p[2], 3, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(arr[4].z, 54, "Member variable unrelated to schema evolution has changed");
    ASSERT_EQUAL(arr[3].p[3], 4444, "Member variable unrelated to schema evolution has changed");
  });
}
