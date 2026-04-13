#ifndef PODIO_UTILITIES_READERCOMMON_H
#define PODIO_UTILITIES_READERCOMMON_H

#include "podio/podioVersion.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"

namespace podio {

class ReaderCommon {
public:
  /// Get the build version of podio that has been used to write the current
  /// file
  ///
  /// @returns The podio build version
  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

  /// Get the (build) version of a datamodel that has been used to write the
  /// current file
  ///
  /// @param name The name of the datamodel
  ///
  /// @returns The (build) version of the datamodel if available or an empty
  ///          optional
  std::optional<podio::version::Version> currentFileVersion(std::string_view name) const {
    return m_datamodelHolder.getDatamodelVersion(name);
  }

  /// Get the datamodel definition for the given name
  ///
  /// @param name The name of the datamodel
  ///
  /// @returns The high level definition of the datamodel in JSON format
  const std::string_view getDatamodelDefinition(std::string_view name) const {
    return m_datamodelHolder.getDatamodelDefinition(name);
  }

  /// Get all names of the datamodels that are available from this reader
  ///
  /// @returns The names of the datamodels
  std::vector<std::string> getAvailableDatamodels() const {
    return m_datamodelHolder.getAvailableDatamodels();
  }

protected:
  podio::version::Version m_fileVersion{0, 0, 0};
  DatamodelDefinitionHolder m_datamodelHolder{};
};

} // namespace podio

#endif // PODIO_UTILITIES_READERCOMMON_H
