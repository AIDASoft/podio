#include "podio/UserDataCollection.h"
#include "podio/SchemaEvolution.h"

#include <tuple>
#include <vector>

namespace podio {

namespace {
  /**
   * Helper function to loop over all types in the SupportedUserDataTypes to
   * register the UserDataCollection types.
   */
  bool registerUserDataCollections() {
    // Use an IILE here to make sure to do the call exactly once
    const static auto reg = []() {
      std::apply([](auto... x) { std::make_tuple(detail::registerUserDataCollection(x)...); },
                 SupportedUserDataTypes{});
      return true;
    }();
    return reg;
  }

  /**
   * Invoke the registration function for user data collections at least once
   */
  const auto registeredUserData = registerUserDataCollections();
} // namespace

} // namespace podio
